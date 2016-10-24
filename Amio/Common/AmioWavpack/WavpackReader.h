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
	class WavpackReader
	{
	public:
		///
		WavpackReader();

		///
		~WavpackReader();

		///
		enum Error
		{
			kError_NoError						= 0,
			kError_General,		
			kError_UnsupportedBitDepth,
		};
		Error Initialize(const amio::UTF16String& inFileName);

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
		asdk::int32 GetChannelMask() const;

		///
		void WavpackReader::GetChannelIdentities(asdk::uint8 *identities) const;

		///
		asdk::int32 GetBitRate() const;		// In the returned bitrate, 1000 bits per second is considered 1 kbps.

		///
		asdk::int32 GetHybridBitrate() const;		// Hybrid bitrate in bits/sample * 256, otherwise 0

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

		/// Get raw interleaved samples.
		bool ReadSamplesRaw(asdk::int64 inStartByteOffset, asdk::int64 inByteCount, void *inBuffer);

		///
		/// Reading Metadata
		///

		///
		int GetRiffMetadataItemCount() const;

		///
		const asdk::uint8* GetRiffMetadataItem(int inIndex, asdk::int32& outSize) const;

		///
		int GetTagMetadataItemCount() const;

		///
		const asdk::uint8* GetTagMetadataItem(int inIndex, asdk::int32& outSize, bool& outIsBinary) const;

	protected:
		class Impl;
		Impl* mImpl;
	};

} //namespace amio

