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
	class ApeReader
	{
	public:
		///
		ApeReader();

		///
		~ApeReader();

		///
		enum Error
		{
			kError_NoError						= 0,
			kError_General,		
			kError_UnsupportedBitDepth,
		};
		Error Initialize(const amio::UTF8String& inFileName);


		///
		/// File Attributes.
		///

		///
		asdk::int64 GetSampleCount() const;

		///
		asdk::int32 GetSampleRate() const;

		///
		asdk::int32 GetNumChannels() const;

		///
		asdk::int32 GetBitRate() const;		// In the returned bitrate, 1000 bits per second is considered 1 kbps.

		///
		asdk::int32 GetCompressionLevel() const;

		///
		asdk::int32 GetBytesPerSample() const;

		///
		/// Reading Samples
		///
		
		///
		/// Get deinterleaved samples for the zero-based inChannelIndex.
		bool ReadSamples(asdk::int64 inStartSample, asdk::int32 inSampleCount, asdk::int32 inChannelIndex, void *inBuffer);

		///
		/// Reading Metadata
		///

		///
		asdk::int32 GetRawMetadataSize() const;

		///
		asdk::uint8* GetRawMetadataBuffer() const;

		///
		int GetRiffMetadataItemCount() const;

		///
		const asdk::uint8* GetRiffMetadataItem(int inIndex, asdk::int32& outSize) const;

	protected:
		class Impl;
		Impl* mImpl;
	};

} //namespace amio

