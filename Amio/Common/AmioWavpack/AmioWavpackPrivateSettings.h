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

		enum CompressionIndex
		{
			kCompressionIndex_Fast = 0,
			kCompressionIndex_Normal = 1,
			kCompressionIndex_High = 2, 
			kCompressionIndex_ExtraHigh = 3,
			kCompressionIndex_Insane = 4,
			kCompressionIndex_Count = 5,
			kCompressionIndex_Default = kCompressionIndex_Normal
		};
		static int GetCompressionLevel(CompressionIndex inCompressionIndex);

		//
		// Settings that pertain only to our file format.
		//
		void SetCompressionLevel(int inLevel);

		int GetCompressionLevel() const;

		int GetCompressionLevelIndex() const;

		amio::UTF16String GetCompressionQualityString() const;	// A human-readable expression of the quality.

		double GetEstimatedCompressionFactor() const;

		//
		// Transfer the format in a serialized form
		//
		amio::UTF16String GetSerialized() const;
		bool InitializeFromSerialized(const amio::UTF16String& inSettings);

	protected:
		void					SetDefaults();
		int						mCompressionLevel;
	};
} // namespace amio

#endif AMIO_WAVPACK_PRIVATE_SETTINGS_H
