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

namespace amio
{
	class ApeWriter
	{

	public:

		//
		ApeWriter();

		///
		~ApeWriter();

		enum ExtendedError
		{
			kExtendedError_NoError						= 0,

			kExtendedError_General,
			kExtendedError_FileCreate,
			kExtendedError_TooMuchData,
		};

		// Initialize a representation of a file for writing.
		bool Initialize(const amio::UTF8String& inFileName, 
			asdk::int32 inSampleRate,
			asdk::int32 inNumChannels,
			asdk::int32 inBytesPerSample,
			asdk::int64 inSampleCount,
			asdk::int32 inCompressionLevel,
			ExtendedError& outExtendedError);

		///
		asdk::int32			GetSampleRate() const;

		///
		asdk::int32			GetNumChannels() const;

		///
		asdk::int32			GetBytesPerSample() const;

		/// The byte offset should be the total number of bytes provided prior to this call.
		bool WriteSampleBytes(asdk::int64 inStartByte, asdk::int32 inByteCount, void *inBuffer);

		/// Finish and close the file being written, or cancel writing.
		bool FinishWrite(bool inCancelMode, void* inMetadataBuffer, int inMetadataBytes);

	protected:
		class Impl;
		Impl* mImpl;
	};
} // namespace amio
