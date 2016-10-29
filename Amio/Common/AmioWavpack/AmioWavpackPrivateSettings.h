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

#ifndef AMIO_WAVPACK_PRIVATE_SETTINGS_H
#define AMIO_WAVPACK_PRIVATE_SETTINGS_H

#include "AmioSDK.h"

namespace amio 
{
	class AmioWavpackPrivateSettings
	{
	public:
		AmioWavpackPrivateSettings();

		//
		// Settings that pertain only to our file format.
		//
		void SetCompressionMode(int inMode);

		int GetCompressionMode() const;

		amio::UTF16String GetCompressionModeString() const;	// A human-readable expression of the mode.

		void SetAppendApeTagsMode(int inMode);

		int GetAppendApeTagsMode() const;

		void SetHybridBitsPerSample(double inBitsPerSample);

		double GetHybridBitsPerSample() const;

		void SetTotalSamplesPerSecond(int inTotalSamplesPerSecond);

		int GetTotalSamplesPerSecond() const;

		double GetEstimatedCompressionFactor() const;

		int GetMinimumBitrate() const;

		int GetCurrentBitrate() const;

		int GetMaximumBitrate() const;

		void SetCurrentBitrate(int inBitrate);

		amio::UTF16String AmioWavpackPrivateSettings::GetWavpackLibraryVersion() const;

		//
		// Transfer the format in a serialized form
		//
		amio::UTF16String GetSerialized() const;
		bool InitializeFromSerialized(const amio::UTF16String& inSettings);

		//
		// handy utilities not specifically WavPack related
		//

		bool isStandardBitrate (int bitrate) const;
		int nearestStandardBitrate (int bitrate) const;

	protected:
		void					SetDefaults();
		int						mCompressionMode;
		double					mHybridBitsPerSample;
		int						mTotalSamplesPerSecond;
		int						mAppendApeTagsMode;
	};
} // namespace amio

#endif AMIO_WAVPACK_PRIVATE_SETTINGS_H
