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

namespace
{
	const amio::UTF16String kAttributeKey_CompressionLevel = amio::utils::AsciiToUTF16("CompressionLevel");
	const amio::UTF16String kAttributeKey_HybridBitrate = amio::utils::AsciiToUTF16("HybridBitrate");

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
		mCompressionLevel = 2000;	// "normal" compression
		mHybridBitrate = 972;		// 3.8 bits/sample (* 256)
	}

	///
	void AmioWavpackPrivateSettings::SetCompressionLevel(int inLevel)
	{
		mCompressionLevel = inLevel;
	}

	///
	int AmioWavpackPrivateSettings::GetCompressionLevel() const
	{
		return mCompressionLevel;
	}

	///
	int AmioWavpackPrivateSettings::GetHybridBitrate() const
	{
		return mHybridBitrate;
	}

	///
	void AmioWavpackPrivateSettings::SetHybridBitrate(int inBitrate)
	{
		mHybridBitrate = inBitrate;
	}

	/// A human-readable expression of the quality.
	amio::UTF16String AmioWavpackPrivateSettings::GetCompressionQualityString() const
	{
		asdk::int32 level = GetCompressionLevel ();
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

		switch (GetCompressionLevel() / 1000)
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

		serializer.AddAttribute(kAttributeKey_CompressionLevel, static_cast<asdk::int32>(GetCompressionLevel()));
		if (GetHybridBitrate()) serializer.AddAttribute(kAttributeKey_HybridBitrate, static_cast<asdk::int32>(GetHybridBitrate()));
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
		if (serializer.QueryAttribute(kAttributeKey_CompressionLevel, iValue))
		{
			SetCompressionLevel(iValue);
		}
		if (serializer.QueryAttribute(kAttributeKey_HybridBitrate, iValue))
		{
			SetHybridBitrate(iValue);
		}
		return true;
	}

} // namespace amio
