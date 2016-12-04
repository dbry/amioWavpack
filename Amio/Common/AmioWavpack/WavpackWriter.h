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

namespace amio
{
	class WavpackWriter
	{

	public:

		//
		WavpackWriter();

		///
		~WavpackWriter();

		enum ExtendedError
		{
			kExtendedError_NoError						= 0,

			kExtendedError_General,
			kExtendedError_FileCreate,
			kExtendedError_TooMuchData,
		};

		// Initialize a representation of a file for writing.

        // Meaning of the 4-digit "CompressionMode" value that we use in several places,
        // including as a parameter here. Value is decimal "sum" with these fields:

        // 1000 = "fast" mode
        // 2000 = "normal/default" mode
        // 3000 = "high" mode
        // 4000 = "very high" mode
        //  100 = hybrid mode (either lossless or lossy)
        //   10 = wvc mode (makes hybrid into lossless)
        //    n = "extra" mode level 0-6

        // Note that the "extra" mode value is used here, but we do not read it from input files
        // (even though the info is available) because we want to user to set explicitly each time

		bool Initialize(const amio::UTF16String& inFileName, 
			asdk::int32 inSampleRate,
			asdk::int32 inNumChannels,
			asdk::int32 inBytesPerSample,
			asdk::int64 inSampleCount,
			asdk::int32 inChannelMask,
            unsigned char *inChannelIdentities,
            unsigned char *inChannelReorder,
			asdk::int32 inCompressionMode,
			double inHybridBitsPerSample,
			ExtendedError& outExtendedError);

		///
		asdk::int32			GetSampleRate() const;

		///
		asdk::int32			GetNumChannels() const;

		///
		asdk::int32			GetBytesPerSample() const;

		/// The byte offset should be the total number of bytes provided prior to this call.
		bool WriteSampleBytes(asdk::int64 inStartByte, asdk::int32 inSampleCount, void *inBuffer);

		/// Flush file being written. Use after all samples have been written or after RIFF data has been written
		bool FlushWrite(void);

		/// Add metadata to file being written (either riff data to trailing WAV wrapper or tag data to APEv2 tag)
		bool AddRiffMetadata(void* inMetadataBuffer, int inMetadataBytes);
		bool AddTagMetadata(void* inMetadataBuffer, int inMetadataBytes, bool inIsBinary);

		/// Finish and close the file being written, or cancel writing.
		bool FinishWrite(bool inCancelMode);

	protected:
		class Impl;
		Impl* mImpl;
	};
} // namespace amio
