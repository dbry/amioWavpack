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
		void SetCompressionLevel(int inLevel);

		int GetCompressionLevel() const;

		amio::UTF16String GetCompressionQualityString() const;	// A human-readable expression of the quality.

		void SetHybridBitrate(int inBitrate);

		int GetHybridBitrate() const;

		double GetEstimatedCompressionFactor() const;

		//
		// Transfer the format in a serialized form
		//
		amio::UTF16String GetSerialized() const;
		bool InitializeFromSerialized(const amio::UTF16String& inSettings);

	protected:
		void					SetDefaults();
		int						mCompressionLevel;
		int						mHybridBitrate;
	};
} // namespace amio

#endif AMIO_WAVPACK_PRIVATE_SETTINGS_H
