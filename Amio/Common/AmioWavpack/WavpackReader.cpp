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
#include "AmioUtilities.h"

#include "windows.h"    // for OutputDebugStringA ()

#include "../Wavpack/include/wavpack.h"
#include "WavpackReader.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

namespace 
{
	template<typename T>
	void DeInterleave(const asdk::int32* inSrc, T* inDest,
			asdk::int32 sampleCount, asdk::int32 skip, int offset = 0)
	{
		const asdk::int32* src = inSrc;
		T* dest = inDest;

		while (sampleCount--)
		{
			*dest++ = static_cast<T>(*src + offset);
			src += skip;
		}
	}

	///
	void Deinterleave24Bits(const asdk::int32* inSrc, void* inDest,
			asdk::int32 sampleCount, asdk::int32 skip)
	{
		const asdk::int32* src = inSrc;
		asdk::uint8* dest = reinterpret_cast<asdk::uint8*>(inDest);
		while (sampleCount--)
		{
            asdk::int32 sam = *src;
			*dest++ = static_cast<asdk::uint8>(sam);
			*dest++ = static_cast<asdk::uint8>(sam >> 8);
			*dest++ = static_cast<asdk::uint8>(sam >> 16);
			src += skip;
		}
	}

	// This structure describes an item of tag metadata found in the media file (from an APEv2 tag).
	struct TagMetadataItem
	{
		inline TagMetadataItem(asdk::uint8* inData, asdk::int32 inSize, bool inIsBinary)
			: mData(inData)
			, mSize(inSize)
			, mIsBinary(inIsBinary)
		{
		}

		asdk::uint8* mData;					// The actual data of this metadata item
		asdk::int32	mSize;					// The size in bytes of this metadata item.
		bool mIsBinary;						// Indicates that the item is a binary tag
	};

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
	class WavpackReader::Impl
	{
	public:
		///
		Impl()
			: mExpectedSeekPosition(0)
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
            wpcx = NULL;
            error [0] = 0;
		}

		///
		~Impl()
		{
			Close();
		}

		///
		void Close()
		{
//          OutputDebugStringA ("WavpackReader::Close() called\n");

            if (wpcx) {
                WavpackCloseFile (wpcx);
                wpcx = NULL;
            }

			BufferSizeReset(0);
			mExpectedSeekPosition = 0;
			mRiffMetadataItems.clear();
			for (int n = 0; n < mTagMetadataItems.size(); ++n) delete mTagMetadataItems[n].mData;
			mTagMetadataItems.clear();
		}

		///
		Error Initialize(const amio::UTF16String& inFileName)
		{
			Close();

            UTF8String inFileNameUTF8 = amio::utils::UTF16StringtoUTF8String (inFileName);

            // This used to use an ifstream and the version of WavPack file open() that uses
            // callbacks. However, I discovered that the MSVC 2008 version of ifstream did not
            // handle 64-bit file pointers (!) and so could not handle the large files that were
            // the very purpose of this update. However, since I had recently added the ability
            // to open by UTF8 filenames, this became an easy solution (the only reason we were
            // using the callback method before was because of Unicode filenames).

            wpcx = WavpackOpenFileInput (reinterpret_cast<const char *>(inFileNameUTF8.c_str()),
                error, OPEN_WVC | OPEN_WRAPPER | OPEN_TAGS | OPEN_NORMALIZE | OPEN_FILE_UTF8 | OPEN_DSD_AS_PCM, 0);

			if (wpcx == NULL)
			{
                Close();
				return kError_General;
			}

			mMode = WavpackGetMode (wpcx);
		    mSampleRate = WavpackGetSampleRate (wpcx);
			mBytesPerSample = WavpackGetBytesPerSample (wpcx);
			int bitsPerSample = WavpackGetBitsPerSample (wpcx);
			if (bitsPerSample > 8*mBytesPerSample || bitsPerSample <= 8*(mBytesPerSample-1))
			{
                Close();
				return kError_UnsupportedBitDepth;
			}
			if (bitsPerSample > 24 && !(mMode & MODE_FLOAT))
			{
                Close();
				return kError_UnsupportedBitDepth;
			}
			mChannels = WavpackGetNumChannels (wpcx);
            mChannelMask = WavpackGetChannelMask (wpcx);
			mAverageBitRate = static_cast<int>(WavpackGetAverageBitrate (wpcx, 1)); 
			mSampleCount = WavpackGetNumSamples64 (wpcx); 

            if (WavpackGetWrapperBytes (wpcx)) {
                WavpackFreeWrapper (wpcx);
            }

            WavpackSeekTrailingWrapper (wpcx);
			mMetadataSize = WavpackGetWrapperBytes (wpcx); 

			if (mMetadataSize)
			{
                char msg [256];
				mMetadataBuffer.reset(new asdk::uint8[mMetadataSize]);
                memcpy (mMetadataBuffer.get(), WavpackGetWrapperData (wpcx), mMetadataSize);
                WavpackFreeWrapper (wpcx);

                sprintf (msg, "WavpackReader::Initialize() got %u bytes of metadata\n", mMetadataSize);
                OutputDebugStringA (msg);

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
                        sprintf (msg, "WavpackReader::Initialize() got RIFF overrun, bufferOffset = %d, bytesLeft = %d, size = %u\n",
                            bufferOffset, bytesLeft, size);
                        OutputDebugStringA (msg);

//						if (bufferOffset == 0)
//						{							
							break;	// Well, maybe this isn't RIFF metadata after all, we'd better ignore it.
//						}
					}

//                  sprintf (msg, "    %c%c%c%c - %d bytes\n",
//                      mMetadataBuffer.get()[bufferOffset+0],
//                      mMetadataBuffer.get()[bufferOffset+1],
//                      mMetadataBuffer.get()[bufferOffset+2],
//                      mMetadataBuffer.get()[bufferOffset+3],
//                      size);
//                  OutputDebugStringA (msg);

					mRiffMetadataItems.push_back(MetadataItem(size, bufferOffset));
					if (size & 1)
						size++;		// RIFF chunks are word aligned.
					bufferOffset += size;
				}
			}

			if (mMode & MODE_VALID_TAG) {
				int binary_items = WavpackGetNumBinaryTagItems (wpcx);
				int text_items = WavpackGetNumTagItems (wpcx);
				asdk::int32 item_len, value_len;
				asdk::uint8* data;
				char *item;
				int i;

				for (i = 0; i < text_items; ++i) {
					item_len = WavpackGetTagItemIndexed (wpcx, i, NULL, 0);
					item = new char [item_len + 1];
					WavpackGetTagItemIndexed (wpcx, i, item, item_len + 1);
					value_len = WavpackGetTagItem (wpcx, item, NULL, 0);

					if (value_len) {
						data = new asdk::uint8 [item_len + 1 + value_len + 1];
						memcpy (data, item, item_len + 1);
						WavpackGetTagItem (wpcx, item, (char *) data + item_len + 1, value_len + 1);
						mTagMetadataItems.push_back(TagMetadataItem (data, item_len + 1 + value_len, false));
					}

					delete item;
				}

				for (i = 0; i < binary_items; ++i) {
					item_len = WavpackGetBinaryTagItemIndexed (wpcx, i, NULL, 0);
					item = new char [item_len + 1];
					WavpackGetBinaryTagItemIndexed (wpcx, i, item, item_len + 1);
					value_len = WavpackGetBinaryTagItem (wpcx, item, NULL, 0);

					if (value_len) {
						data = new asdk::uint8 [item_len + 1 + value_len];
						memcpy (data, item, item_len + 1);
						WavpackGetBinaryTagItem (wpcx, item, (char *) data + item_len + 1, value_len);
						mTagMetadataItems.push_back(TagMetadataItem (data, item_len + 1 + value_len, true));
					}

					delete item;
				}

#if 0
				for (i = 0; i < mTagMetadataItems.size(); ++i) {
					int item_len = (int) strlen ((char *) mTagMetadataItems[i].mData);
					char msg [256];

					if (mTagMetadataItems[i].mIsBinary)
						sprintf (msg, "    \"%s\" (%d) = %d bytes of binary data\n", (char *) mTagMetadataItems[i].mData,
							item_len, mTagMetadataItems[i].mSize - item_len - 1);
					else
						sprintf (msg, "    \"%s\" (%d) = \"%s\"\n", (char *) mTagMetadataItems[i].mData,
							item_len, (char *) mTagMetadataItems[i].mData + item_len + 1);

					OutputDebugStringA (msg);
				}
#endif
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
				if (!BufferSizeReset(inSampleCount * mChannels * 4))
				{
					return false;
				}
				if (inStartSample != mExpectedSeekPosition)
				{
					WavpackSeekSample (wpcx, static_cast<int>(static_cast<asdk::uint32>(inStartSample)));
				}

				int blocksRead = WavpackUnpackSamples (wpcx, reinterpret_cast<int32_t*>(mInterleavedBuffer.get ()), inSampleCount);

				if (blocksRead != inSampleCount)
				{
                    OutputDebugStringA ("WavpackReader::ReadSamples() read incomplete buffer!!\n");
					return false;
				}
				mExpectedSeekPosition = inStartSample + inSampleCount;
				mInterleavedBufferSampleStart = inStartSample;
				mInterleavedBufferSampleBlocksValid = inSampleCount;
			}

			int byteOffsetIntoBuffer = (static_cast<int>(inStartSample - mInterleavedBufferSampleStart) + inChannelIndex) * 4;
			switch (mBytesPerSample)
			{
			case 1: 
				DeInterleave<asdk::uint8>(reinterpret_cast<asdk::int32*>(mInterleavedBuffer.get() + byteOffsetIntoBuffer), 
						static_cast<asdk::uint8*>(inBuffer),
						inSampleCount, mChannels, 128);
				break;
			case 2:
				DeInterleave<asdk::int16>(reinterpret_cast<asdk::int32*>(mInterleavedBuffer.get() + byteOffsetIntoBuffer), 
						static_cast<asdk::int16*>(inBuffer),
						inSampleCount, mChannels);
				break;
			case 3:
				Deinterleave24Bits(reinterpret_cast<asdk::int32*>(mInterleavedBuffer.get() + byteOffsetIntoBuffer), 
						static_cast<asdk::uint8*>(inBuffer),
						inSampleCount, mChannels);
				break;
			case 4:
				DeInterleave<asdk::int32>(reinterpret_cast<asdk::int32*>(mInterleavedBuffer.get() + byteOffsetIntoBuffer), 
						static_cast<asdk::int32*>(inBuffer),
						inSampleCount, mChannels);
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
		int GetRiffMetadataItemCount() const
		{
			return (int) mRiffMetadataItems.size();
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

		///
		int GetTagMetadataItemCount() const
		{
			return (int) mTagMetadataItems.size();
		}

		///
		const asdk::uint8* GetTagMetadataItem(int inIndex, asdk::int32& outSize, bool& outIsBinary)
		{
			if (inIndex >= GetTagMetadataItemCount())
			{
				return NULL;
			}
			outSize = mTagMetadataItems[inIndex].mSize;
			outIsBinary = mTagMetadataItems[inIndex].mIsBinary;
			return mTagMetadataItems[inIndex].mData;
		}


	protected:
        WavpackContext                  *wpcx;
        char                            error [80];
		asdk::int64						mExpectedSeekPosition;
		std::auto_ptr<char>				mInterleavedBuffer;
		int								mInterleavedBufferSizeInBytes;
		asdk::int64						mInterleavedBufferSampleStart;
		asdk::int32						mInterleavedBufferSampleBlocksValid;
		std::auto_ptr<asdk::uint8>		mMetadataBuffer;
		std::vector<MetadataItem>		mRiffMetadataItems;
		std::vector<TagMetadataItem>	mTagMetadataItems;

	public:
        int             mMode;
		int				mSampleRate;
		int				mChannels;
		asdk::int32		mChannelMask;
		int				mBytesPerSample;
		int				mAverageBitRate;
		asdk::int64		mSampleCount;
		int				mMetadataSize;
	};


	///
	WavpackReader::WavpackReader()
	{
		mImpl = new WavpackReader::Impl();
	}

	///
	WavpackReader::~WavpackReader()
	{
		delete mImpl;
	}

	///
	WavpackReader::Error WavpackReader::Initialize(const amio::UTF16String& inFileName)
	{
		return mImpl->Initialize(inFileName);
	}
	
	asdk::int64 WavpackReader::GetSampleCount() const
	{
		return mImpl->mSampleCount;
	}

	///
	asdk::int32 WavpackReader::GetSampleRate() const
	{
		return mImpl->mSampleRate;
	}

	///
	asdk::int32 WavpackReader::GetNumChannels() const
	{
		return mImpl->mChannels;
	}

	///
	asdk::int32 WavpackReader::GetChannelMask() const
	{
		return mImpl->mChannelMask;
	}

	///
	asdk::int32 WavpackReader::GetBitRate() const
	{
		return mImpl->mAverageBitRate;
	}

	///
	asdk::int32 WavpackReader::GetCompressionLevel() const
	{
        if (mImpl->mMode & MODE_FAST)
            return 1000;
        else if (mImpl->mMode & MODE_VERY_HIGH)
            return 4000;
        else if (mImpl->mMode & MODE_HIGH)
            return 3000;
        else
            return 2000;
	}

	///
	asdk::int32 WavpackReader::GetBytesPerSample() const
	{
		return mImpl->mBytesPerSample;
	}

	/// Get deinterleaved samples for the zero-based inChannelIndex.
	bool WavpackReader::ReadSamples(asdk::int64 inStartSample, asdk::int32 inSampleCount, asdk::int32 inChannelIndex, void *inBuffer)
	{
		return mImpl->ReadSamples(inStartSample, inSampleCount, inChannelIndex, inBuffer);
	}

	///
	int WavpackReader::GetRiffMetadataItemCount() const
	{
		return mImpl->GetRiffMetadataItemCount();
	}

	///
	const asdk::uint8* WavpackReader::GetRiffMetadataItem(int inIndex, asdk::int32& outSize) const
	{
		return mImpl->GetRiffMetadataItem(inIndex, outSize);
	}

	///
	int WavpackReader::GetTagMetadataItemCount() const
	{
		return mImpl->GetTagMetadataItemCount();
	}

	///
	const asdk::uint8* WavpackReader::GetTagMetadataItem(int inIndex, asdk::int32& outSize, bool& outIsBinary) const
	{
		return mImpl->GetTagMetadataItem(inIndex, outSize, outIsBinary);
	}

} //namespace amio

