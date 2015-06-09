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

#include "AmioApePrivateSettings.h"
#include "AmioPrivateSettingsSerializer.h"
#include "AmioUtilities.h"

namespace
{
	const amio::UTF16String kAttributeKey_CompressionLevel = amio::utils::AsciiToUTF16("CompressionLevel");

} // private namespace

namespace amio 
{
	///
	AmioApePrivateSettings::AmioApePrivateSettings()
	{
		SetDefaults();
	}

	enum CompressionIndex
	{
		kCompressionIndex_Fast = 0,
		kCompressionIndex_Normal = 1,
		kCompressionIndex_High = 2, 
		kCompressionIndex_VeryHigh = 3,
		kCompressionIndex_Count = 4,
		kCompressionIndex_Default = kCompressionIndex_Normal
	};
	int AmioApePrivateSettings::GetCompressionLevel(CompressionIndex inCompressionIndex)
	{
	// From MacLib.h
	// #define COMPRESSION_LEVEL_FAST          1000
	// #define COMPRESSION_LEVEL_NORMAL        2000
	// #define COMPRESSION_LEVEL_HIGH          3000
	// #define COMPRESSION_LEVEL_EXTRA_HIGH    4000

		switch(inCompressionIndex)
		{
		case kCompressionIndex_Fast:
			return 1000;
		default:
		case kCompressionIndex_Normal:
			return 2000;
		case kCompressionIndex_High:
			return 3000;
		case kCompressionIndex_VeryHigh:
			return 4000;
		}
	}

	///
	void AmioApePrivateSettings::SetDefaults()
	{
		mCompressionLevel = GetCompressionLevel(kCompressionIndex_Default);
	}

	///
	void AmioApePrivateSettings::SetCompressionLevel(int inLevel)
	{
		mCompressionLevel = inLevel;
	}

	///
	int AmioApePrivateSettings::GetCompressionLevel() const
	{
		return mCompressionLevel;
	}

	///
	int AmioApePrivateSettings::GetCompressionLevelIndex() const
	{
		if (mCompressionLevel <= GetCompressionLevel(kCompressionIndex_Fast))
			return kCompressionIndex_Fast;
		if (mCompressionLevel <= GetCompressionLevel(kCompressionIndex_Normal))
			return kCompressionIndex_Normal;
		if (mCompressionLevel <= GetCompressionLevel(kCompressionIndex_High))
			return kCompressionIndex_High;
        return kCompressionIndex_VeryHigh;
	}

	/// A human-readable expression of the quality.
	amio::UTF16String AmioApePrivateSettings::GetCompressionQualityString() const
	{
		switch (GetCompressionLevelIndex())
		{
		case kCompressionIndex_Fast:
			return amio::utils::AsciiToUTF16("Fast Compression");
		default:
		case kCompressionIndex_Normal:
			return amio::utils::AsciiToUTF16("Normal Compression");
		case kCompressionIndex_High:
			return amio::utils::AsciiToUTF16("High Compression");
		case kCompressionIndex_VeryHigh:
			return amio::utils::AsciiToUTF16("Very High Compression");
		}
	}

	///
	double AmioApePrivateSettings::GetEstimatedCompressionFactor() const
	{
		const double conservativeAdjustment = 1.125;

		switch (GetCompressionLevelIndex())
		{
		case kCompressionIndex_Fast:			return 0.569 * conservativeAdjustment;
		default:							
		case kCompressionIndex_Normal:			return 0.545 * conservativeAdjustment;
		case kCompressionIndex_High:			return 0.536 * conservativeAdjustment;
		case kCompressionIndex_VeryHigh:		return 0.527 * conservativeAdjustment;
		}
	}

	///
	amio::UTF16String AmioApePrivateSettings::GetSerialized() const
	{
		AmioPrivateSettingsSerializer serializer;

		serializer.AddAttribute(kAttributeKey_CompressionLevel, static_cast<asdk::int32>(GetCompressionLevel()));
		amio::UTF16String ret = serializer.GetSerializedString();
		return ret;
	}

	///
	bool AmioApePrivateSettings::InitializeFromSerialized(const amio::UTF16String& inSettings)
	{
		SetDefaults();
		AmioPrivateSettingsSerializer serializer;
		serializer.Initialize(inSettings);

		asdk::int32 iValue;
		if (serializer.QueryAttribute(kAttributeKey_CompressionLevel, iValue))
		{
			SetCompressionLevel(iValue);
		}
		return true;
	}

} // namespace amio
