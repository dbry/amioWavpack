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

// Amio SDK
#include "ApeTypes.h"
#include "All.h"
#include "MACLib.h"

#include "ApeWriter.h"

namespace amio
{
	class ApeWriter::Impl
	{
	public:
		///
		Impl()
			: mSampleRate(0)
			, mChannelCount(0)
			, mBytesPerSample(0)
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
			mCompressor.reset();
			mSampleRate = 0;
			mChannelCount = 0;
			mBytesPerSample = 0;
		}

		bool Initialize(const amio::UTF8String& inFileName, 
			asdk::int32 inSampleRate,
			asdk::int32 inNumChannels,
			asdk::int32 inBytesPerSample,
			asdk::int64 inSampleCount,
			asdk::int32 inCompressionLevel,
			ExtendedError& outExtendedError)
		{
			asdk::int64 totalInputBytes = inSampleCount * inNumChannels * inBytesPerSample;
			if (totalInputBytes > 0x000000007FFFFFFFLL)
			{
				outExtendedError = kExtendedError_TooMuchData;
				return false;
			}

			int errorCode = 0;
			std::auto_ptr<IAPECompress> compressor(CreateIAPECompress(&errorCode));

			WAVEFORMATEX waveFormatEx = {
				1, 
				static_cast<WORD>(inNumChannels),
				inSampleRate,
				inSampleRate * inBytesPerSample * inNumChannels,
				static_cast<WORD>(inBytesPerSample * inNumChannels),
				static_cast<WORD>(inBytesPerSample * 8),
				0};

			int apeError = compressor->Start(reinterpret_cast<const char*>(inFileName.c_str()), 
				&waveFormatEx, 
				static_cast<int>(totalInputBytes),
				inCompressionLevel, 
				NULL, 
				CREATE_WAV_HEADER_ON_DECOMPRESSION);

			switch (apeError)
			{
			case ERROR_INVALID_OUTPUT_FILE:
				outExtendedError = kExtendedError_FileCreate;
				return false;
			default:
				outExtendedError = kExtendedError_General;
				return false;
			case ERROR_SUCCESS:
				break;
			}
			
			mCompressor = compressor;
			mSampleRate = inSampleRate;
			mChannelCount = inNumChannels;
			mBytesPerSample = inBytesPerSample;

			outExtendedError = kExtendedError_NoError;
			return true;
		}

		bool WriteSampleBytes(asdk::int64 inStartByte, asdk::int32 inByteCount, void *inBuffer)
		{
			if (mCompressor.get() == NULL)
				return false;

			int errorCode = mCompressor->AddData(static_cast<unsigned char*>(inBuffer), inByteCount);
			return (errorCode == ERROR_SUCCESS);
		}

		/// Finish and close the file being written, or cancel writing.
		bool FinishWrite(bool inCancelMode, void* inMetadataBuffer, int inMetadataBytes)
		{
			int errorCode = ERROR_SUCCESS;
			if (!inCancelMode)
			{
				errorCode = mCompressor->Finish(static_cast<unsigned char*>(inMetadataBuffer), inMetadataBytes, inMetadataBytes);
			}
			Close();
			return (errorCode == ERROR_SUCCESS);
		}

	protected:
		std::auto_ptr<IAPECompress>	mCompressor;
	public:
		asdk::int32 mSampleRate;
		asdk::int32 mChannelCount;
		asdk::int32 mBytesPerSample;
	};


	///
	ApeWriter::ApeWriter()
	{
		mImpl = new ApeWriter::Impl();
	}

	///
	ApeWriter::~ApeWriter()
	{
		delete mImpl;
	}


	bool ApeWriter::Initialize(const amio::UTF8String& inFileName, 
		asdk::int32 inSampleRate,
		asdk::int32 inNumChannels,
		asdk::int32 inBytesPerSample,
		asdk::int64 inSampleCount,
		asdk::int32 inCompressionLevel,
		ExtendedError& outExtendedError)
	{
		return mImpl->Initialize(inFileName, 
			inSampleRate, 
			inNumChannels, 
			inBytesPerSample, 
			inSampleCount, 
			inCompressionLevel,
			outExtendedError);
	}

	///
	asdk::int32 ApeWriter::GetSampleRate() const
	{
		return mImpl->mSampleRate;
	}

	///
	asdk::int32 ApeWriter::GetNumChannels() const
	{
		return mImpl->mChannelCount;
	}

	///
	asdk::int32 ApeWriter::GetBytesPerSample() const
	{
		return mImpl->mBytesPerSample;
	}

	/// The byte offset should be the total number of bytes provided prior to this call.
	bool ApeWriter::WriteSampleBytes(asdk::int64 inStartByte, asdk::int32 inByteCount, void *inBuffer)
	{
		return mImpl->WriteSampleBytes(inStartByte, inByteCount, inBuffer);
	}

	// Finish and close the file being written, or cancel writing.
	bool ApeWriter::FinishWrite(bool inCancelMode, void* inMetadataBuffer, int inMetadataBytes)
	{
		return mImpl->FinishWrite(inCancelMode, inMetadataBuffer, inMetadataBytes);
	}

} //namespace amio

