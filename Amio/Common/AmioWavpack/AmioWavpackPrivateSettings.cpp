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

#include "AmioWavpackPrivateSettings.h"
#include "AmioPrivateSettingsSerializer.h"
#include "AmioUtilities.h"

#include "../Wavpack/include/wavpack.h"		// for WavpackGetLibraryVersionString() only (no context)

namespace
{
	const amio::UTF16String kAttributeKey_CompressionMode = amio::utils::AsciiToUTF16("CompressionMode");
	const amio::UTF16String kAttributeKey_HybridBitsPerSample = amio::utils::AsciiToUTF16("HybridBitsPerSample");
	const amio::UTF16String kAttributeKey_AppendApeTagsMode = amio::utils::AsciiToUTF16("AppendApeTagsMode");

} // private namespace

namespace amio 
{
	///
	AmioWavpackPrivateSettings::AmioWavpackPrivateSettings()
	{
		SetDefaults();
	}

	///
	void AmioWavpackPrivateSettings::SetDefaults()
	{
		mCompressionMode = 2000;		// "normal" lossless compression
		mHybridBitsPerSample = 3.8;		// 3.8 bits/sample
		mAppendApeTagsMode = 0;			// default is no APE tags present
	}

	///
	void AmioWavpackPrivateSettings::SetCompressionMode(int inMode)
	{
		mCompressionMode = inMode;

		// if not hybrid (i.e., lossless) then default the bitrate
		if (!((mCompressionMode % 1000) - (mCompressionMode % 100)))
			mHybridBitsPerSample = 3.8;		// 3.8 bits/sample

		// if we know samples per second then refine bitrate to a "standard"
		if (mTotalSamplesPerSecond)
			SetCurrentBitrate (nearestStandardBitrate (GetCurrentBitrate()));
	}

	///
	int AmioWavpackPrivateSettings::GetCompressionMode() const
	{
		return mCompressionMode;
	}

	///
	void AmioWavpackPrivateSettings::SetAppendApeTagsMode(int inMode)
	{
		mAppendApeTagsMode = inMode;
	}

	///
	int AmioWavpackPrivateSettings::GetAppendApeTagsMode() const
	{
		return mAppendApeTagsMode;
	}

	///
	double AmioWavpackPrivateSettings::GetHybridBitsPerSample() const
	{
		return mHybridBitsPerSample;
	}

	///
	void AmioWavpackPrivateSettings::SetHybridBitsPerSample(double inBitsPerSample)
	{
		if (inBitsPerSample != 0.0)
			mHybridBitsPerSample = inBitsPerSample;
	}

	///
	void AmioWavpackPrivateSettings::SetCurrentBitrate(int inBitrate)
	{
		if (inBitrate)
			mHybridBitsPerSample = inBitrate * 1000.0 / mTotalSamplesPerSecond;
	}

	///
	int AmioWavpackPrivateSettings::GetTotalSamplesPerSecond() const
	{
		return mTotalSamplesPerSecond;
	}

	///
	void AmioWavpackPrivateSettings::SetTotalSamplesPerSecond(int inTotalSamplesPerSecond)
	{
		mTotalSamplesPerSecond = inTotalSamplesPerSecond;
	}

	int AmioWavpackPrivateSettings::GetMinimumBitrate() const
	{
		return (mTotalSamplesPerSecond + 225) / 450;	// 2.22 bits/sample
	}

	int AmioWavpackPrivateSettings::GetCurrentBitrate() const
	{
		return static_cast<int>(mTotalSamplesPerSecond * mHybridBitsPerSample / 1000.0 + 0.5);
	}

	int AmioWavpackPrivateSettings::GetMaximumBitrate() const
	{
		return (mTotalSamplesPerSecond + 45) / 90;		// 11.11 bits/sample
	}

	/// A human-readable expression of the mode, e.g. "Hybrid Lossy Fast" or "Lossless"
	amio::UTF16String AmioWavpackPrivateSettings::GetCompressionModeString() const
	{
		asdk::int32 level = GetCompressionMode ();
		amio::UTF16String ret_string;

		if ((level % 1000) - (level % 100)) {
			if ((level % 100) - (level % 10))
				ret_string = amio::utils::AsciiToUTF16("Hybrid Lossless");
			else
				ret_string = amio::utils::AsciiToUTF16("Hybrid Lossy");
		}
		else
			ret_string = amio::utils::AsciiToUTF16("Lossless");

		switch (level / 1000)
		{
		case 1:
			ret_string += amio::utils::AsciiToUTF16(" Fast");
			break;

		case 3:
			ret_string += amio::utils::AsciiToUTF16(" High");
			break;

		case 4:
			ret_string += amio::utils::AsciiToUTF16(" Very High");
			break;
		}

		return ret_string;
	}

	///
	double AmioWavpackPrivateSettings::GetEstimatedCompressionFactor() const
	{
		const double conservativeAdjustment = 1.125;

		switch (GetCompressionMode() / 1000)
		{
		case 1:			return 0.569 * conservativeAdjustment;
		default:							
		case 2:			return 0.545 * conservativeAdjustment;
		case 3:			return 0.536 * conservativeAdjustment;
		case 4:			return 0.527 * conservativeAdjustment;
		}
	}

	///
	amio::UTF16String AmioWavpackPrivateSettings::GetSerialized() const
	{
		AmioPrivateSettingsSerializer serializer;

		serializer.AddAttribute(kAttributeKey_CompressionMode, static_cast<asdk::int32>(GetCompressionMode()));
		if ((mCompressionMode % 1000) - (mCompressionMode % 100))
			serializer.AddAttribute(kAttributeKey_HybridBitsPerSample, static_cast<asdk::int32>(GetHybridBitsPerSample() * 1000.0 + 0.5));
		if (GetAppendApeTagsMode())
			serializer.AddAttribute(kAttributeKey_AppendApeTagsMode, static_cast<asdk::int32>(GetAppendApeTagsMode()));
		amio::UTF16String ret = serializer.GetSerializedString();
		return ret;
	}

	///
	bool AmioWavpackPrivateSettings::InitializeFromSerialized(const amio::UTF16String& inSettings)
	{
		SetDefaults();
		AmioPrivateSettingsSerializer serializer;
		serializer.Initialize(inSettings);

		asdk::int32 iValue;
		if (serializer.QueryAttribute(kAttributeKey_CompressionMode, iValue))
		{
			SetCompressionMode(iValue);
		}
		if ((mCompressionMode % 1000) - (mCompressionMode % 100) &&
		    serializer.QueryAttribute(kAttributeKey_HybridBitsPerSample, iValue))
		{
			SetHybridBitsPerSample(iValue / 1000.0);
		}
		if (serializer.QueryAttribute(kAttributeKey_AppendApeTagsMode, iValue))
		{
			SetAppendApeTagsMode(iValue);
		}
		return true;
	}

	///
	amio::UTF16String AmioWavpackPrivateSettings::GetWavpackLibraryVersion() const
	{
		return amio::utils::AsciiToUTF16 (WavpackGetLibraryVersionString ());
	}

	//
	// handy utilities not specifically WavPack related
	//

	bool AmioWavpackPrivateSettings::isStandardBitrate (int bitrate) const
	{
		int sd, sc = 0;

		for (sd = bitrate; sd > 7; sd >>= 1)
			sc++;

		return bitrate == sd << sc;
	}

	int AmioWavpackPrivateSettings::nearestStandardBitrate (int bitrate) const
	{
        for (int delta = 1; ; delta++)
            if (isStandardBitrate (bitrate - delta))
                return bitrate - delta;
            else if (isStandardBitrate (bitrate + delta))
                return bitrate + delta;
	}

} // namespace amio
