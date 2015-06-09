/**************************************************************************
// ADOBE SYSTEMS INCORPORATED
// Copyright  2008 - 2011 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this 
// file in accordance with the terms of the Adobe license agreement
// accompanying it.  If you have received this file from a source
// other than Adobe, then your use, modification, or distribution
// of it requires the prior written permission of Adobe.
**************************************************************************/

#include "AmioSDK.h"
#include "AmioInterfaceTemplate.h"
#include "AmioUtilities.h"

#include "AmioApePrivateSettings.h"
#include "AmioApeSettingsDialog.h"

#include "ApeTypes.h"
#include "All.h"
#include "APETag.h"

#include "ApeReader.h"

#include <string>

namespace 
{
	template<typename T>
	void DeInterleave(const T* inSrc, T* inDest,
			asdk::int32 sampleCount, asdk::int32 skip)
	{
		const T* src = inSrc;
		T* dest = inDest;

		while (sampleCount--)
		{
			*dest++ = *src;
			src += skip;
		}
	}

	///
	void Deinterleave24Bits(void* inSrc, void* inDest,
			asdk::int32 sampleCount, asdk::int32 skip)
	{
		asdk::uint8* src = reinterpret_cast<asdk::uint8*>(inSrc);
		asdk::uint8* dest = reinterpret_cast<asdk::uint8*>(inDest);
		skip *= 3;
		while (sampleCount--)
		{
			*dest++ = *src;
			*dest++ = *(src+1);
			*dest++ = *(src+2);
			src += skip;
		}
	}

	// This structure describes an item of metadata found in the media file.
	struct MetadataItem
	{
		inline MetadataItem()
			: mSize(0)
			, mRawBufferOffset(0)
		{
		}
			
		inline MetadataItem(asdk::int32 inSize, asdk::int32 inBufferOffset)
			: mSize(inSize)
			, mRawBufferOffset(inBufferOffset)
		{
		}

		asdk::int32	mSize;					// The size in bytes of this metadata item.
		asdk::int32	mRawBufferOffset;		// The offset into the raw metadata buffer where this item starts.
	};

} // private namespace

namespace amio
{
	class ApeReader::Impl
	{
	public:
		///
		Impl()
			: mExpectedSeekPosition(0)
			, mCompressionLevel(0)
			, mSampleRate(0)
			, mChannels(0)
			, mBytesPerSample(0)
			, mAverageBitRate(0)
			, mSampleCount(0)
			, mInterleavedBufferSizeInBytes(0)
			, mInterleavedBufferSampleStart(0)
			, mInterleavedBufferSampleBlocksValid(0)
			, mMetadataSize(0)
		{
		}

		///
		~Impl()
		{
			Close();
		}

		///
		void Close()
		{
			BufferSizeReset(0);
			mDecompressor.reset();
			mExpectedSeekPosition = 0;
			mRiffMetadataItems.clear();
		}

		///
		Error Initialize(const amio::UTF8String& inFileName)
		{
			int errorCode = 0;
			mDecompressor.reset(CreateIAPEDecompress(reinterpret_cast<const char*>(inFileName.c_str()), &errorCode));
			if (mDecompressor.get() == NULL)
			{
				return kError_General;
			}

		    mCompressionLevel = mDecompressor->GetInfo(APE_INFO_COMPRESSION_LEVEL, 0, 0);
		    mSampleRate = mDecompressor->GetInfo(APE_INFO_SAMPLE_RATE, 0, 0);
			mBytesPerSample = mDecompressor->GetInfo(APE_INFO_BYTES_PER_SAMPLE, 0, 0);
			int bitsPerSample = mDecompressor->GetInfo(APE_INFO_BITS_PER_SAMPLE, 0, 0);
			if (bitsPerSample != (8*mBytesPerSample))
			{
				return kError_UnsupportedBitDepth;
			}
			if (bitsPerSample > 24)
			{
				return kError_UnsupportedBitDepth;
			}
			mChannels = mDecompressor->GetInfo(APE_INFO_CHANNELS, 0, 0);
			int blockAlign = mDecompressor->GetInfo(APE_INFO_BLOCK_ALIGN, 0, 0);
			if (blockAlign != (mChannels * mBytesPerSample))
			{
				return kError_UnsupportedBitDepth;
			}
			mAverageBitRate = 1000 * mDecompressor->GetInfo(APE_INFO_AVERAGE_BITRATE, 0, 0); 
			unsigned int blockCount = mDecompressor->GetInfo(APE_INFO_TOTAL_BLOCKS, 0, 0); 
			mSampleCount = static_cast<asdk::int64>(blockCount);

			mMetadataSize = mDecompressor->GetInfo(APE_INFO_WAV_TERMINATING_BYTES, 0, 0); 
			if (mMetadataSize)
			{
				mMetadataBuffer.reset(new asdk::uint8[mMetadataSize]);
				mDecompressor->GetInfo(APE_INFO_WAV_TERMINATING_DATA, reinterpret_cast<int>(mMetadataBuffer.get()), mMetadataSize);

				// Look for individual RIFF metadata items.
				int bufferOffset = 0;
				while (1)
				{
					int bytesLeft = mMetadataSize - bufferOffset;
					if (bytesLeft < 8)
						break;
					asdk::uint32 size = 8 + *(reinterpret_cast<asdk::uint32*>(&mMetadataBuffer.get()[bufferOffset+4]));
					if (size > static_cast<asdk::uint32>(bytesLeft))
					{
						if (bufferOffset == 0)
						{							
							break;	// Well, maybe this isn't RIFF metadata after all, we'd better ignore it.
						}
					}
					mRiffMetadataItems.push_back(MetadataItem(size, bufferOffset));
					if (size & 1)
						size++;		// RIFF chunks are word aligned.
					bufferOffset += size;
				}
			}
			return kError_NoError;
		}

		///
		bool ReadSamples(asdk::int64 inStartSample, asdk::int32 inSampleCount, asdk::int32 inChannelIndex, void *inBuffer)
		{
			// If the entire requested portion is not in our buffer, reallocate and decode
			if (!((inStartSample >= mInterleavedBufferSampleStart) && 
					((inStartSample + inSampleCount) <= (mInterleavedBufferSampleStart + mInterleavedBufferSampleBlocksValid))))
			{
				if (!BufferSizeReset(inSampleCount * mChannels * mBytesPerSample))
				{
					return false;
				}
				if (inStartSample != mExpectedSeekPosition)
				{
					mDecompressor->Seek(static_cast<int>(static_cast<asdk::uint32>(inStartSample)));
				}

				int blocksRead = 0;
			    mDecompressor->GetData(static_cast<char*>(mInterleavedBuffer.get()), inSampleCount, &blocksRead);
				if (blocksRead != inSampleCount)
				{
					return false;
				}
				mExpectedSeekPosition = inStartSample + inSampleCount;
				mInterleavedBufferSampleStart = inStartSample;
				mInterleavedBufferSampleBlocksValid = inSampleCount;
			}

			int byteOffsetIntoBuffer = (static_cast<int>(inStartSample - mInterleavedBufferSampleStart) + inChannelIndex) * mBytesPerSample;
			switch (mBytesPerSample)
			{
			case 1: 
				DeInterleave<asdk::uint8>(reinterpret_cast<asdk::uint8*>(mInterleavedBuffer.get() + byteOffsetIntoBuffer), 
						static_cast<asdk::uint8*>(inBuffer),
						inSampleCount, 
						mChannels);
				break;
			case 2:
				DeInterleave<asdk::int16>(reinterpret_cast<asdk::int16*>(mInterleavedBuffer.get() + byteOffsetIntoBuffer), 
						static_cast<asdk::int16*>(inBuffer),
						inSampleCount, 
						mChannels);
				break;
			case 3:
				Deinterleave24Bits(reinterpret_cast<asdk::uint8*>(mInterleavedBuffer.get() + byteOffsetIntoBuffer), 
						static_cast<asdk::uint8*>(inBuffer),
						inSampleCount, 
						mChannels);
				break;
			}
			return true;
		}

		///
		bool BufferSizeReset(int inNewSize)
		{
			if (inNewSize < mInterleavedBufferSizeInBytes)
			{
				return true;	// Just use the existing size.
			}

			// Free the existing buffer.
			mInterleavedBuffer.reset(0);
			mInterleavedBufferSizeInBytes = 0;
			mInterleavedBufferSampleStart = 0;
			mInterleavedBufferSampleBlocksValid = 0;

			if (inNewSize > 0)
			{
				mInterleavedBuffer.reset(new char[inNewSize]);
				mInterleavedBufferSizeInBytes = inNewSize;
			}
			return true;
		}

		///
		asdk::uint8* GetMetadataBuffer() const
		{
			return mMetadataBuffer.get();
		}

		///
		int GetRiffMetadataItemCount() const
		{
			return mRiffMetadataItems.size();
		}

		///
		const asdk::uint8* GetRiffMetadataItem(int inIndex, asdk::int32& outSize)
		{
			if (!mMetadataBuffer.get())
			{
				return NULL;
			}
			if (inIndex >= GetRiffMetadataItemCount())
			{
				return NULL;
			}
			outSize = mRiffMetadataItems[inIndex].mSize;
			return (&mMetadataBuffer.get()[mRiffMetadataItems[inIndex].mRawBufferOffset]);
		}


	protected:
		std::auto_ptr<IAPEDecompress>	mDecompressor;
		asdk::int64						mExpectedSeekPosition;
		std::auto_ptr<char>				mInterleavedBuffer;
		int								mInterleavedBufferSizeInBytes;
		asdk::int64						mInterleavedBufferSampleStart;
		asdk::int32						mInterleavedBufferSampleBlocksValid;
		std::auto_ptr<asdk::uint8>		mMetadataBuffer;
		std::vector<MetadataItem>		mRiffMetadataItems;

	public:
		int				mCompressionLevel;
		int				mSampleRate;
		int				mChannels;
		int				mBytesPerSample;
		int				mAverageBitRate;
		asdk::int64		mSampleCount;
		int				mMetadataSize;
	};


	///
	ApeReader::ApeReader()
	{
		mImpl = new ApeReader::Impl();
	}

	///
	ApeReader::~ApeReader()
	{
		delete mImpl;
	}

	///
	ApeReader::Error ApeReader::Initialize(const amio::UTF8String& inFileName)
	{
		return mImpl->Initialize(inFileName);
	}
	
	asdk::int64 ApeReader::GetSampleCount() const
	{
		return mImpl->mSampleCount;
	}

	///
	asdk::int32 ApeReader::GetSampleRate() const
	{
		return mImpl->mSampleRate;
	}

	///
	asdk::int32 ApeReader::GetNumChannels() const
	{
		return mImpl->mChannels;
	}

	///
	asdk::int32 ApeReader::GetBitRate() const
	{
		return mImpl->mAverageBitRate;
	}

	///
	asdk::int32 ApeReader::GetCompressionLevel() const
	{
		return mImpl->mCompressionLevel;
	}

	///
	asdk::int32 ApeReader::GetBytesPerSample() const
	{
		return mImpl->mBytesPerSample;
	}

	/// Get deinterleaved samples for the zero-based inChannelIndex.
	bool ApeReader::ReadSamples(asdk::int64 inStartSample, asdk::int32 inSampleCount, asdk::int32 inChannelIndex, void *inBuffer)
	{
		return mImpl->ReadSamples(inStartSample, inSampleCount, inChannelIndex, inBuffer);
	}

	///
	asdk::int32 ApeReader::GetRawMetadataSize() const
	{
		return mImpl->mMetadataSize;
	}

	///
	asdk::uint8* ApeReader::GetRawMetadataBuffer() const
	{
		return mImpl->GetMetadataBuffer();
	}

	///
	int ApeReader::GetRiffMetadataItemCount() const
	{
		return mImpl->GetRiffMetadataItemCount();
	}

	///
	const asdk::uint8* ApeReader::GetRiffMetadataItem(int inIndex, asdk::int32& outSize) const
	{
		return mImpl->GetRiffMetadataItem(inIndex, outSize);
	}

} //namespace amio

