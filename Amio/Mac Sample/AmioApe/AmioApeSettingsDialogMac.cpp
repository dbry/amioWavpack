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

#include "AmioApeSettingsDialog.h"

// C interface to objective C dialog
extern bool DisplaySettingsDialog(void* inParentWindow, int& inoutSetting);

namespace amio 
{

	///	
	AmioApeSettingsDialog::AmioApeSettingsDialog(AmioApePrivateSettings& inSettings)
		: mSettings(inSettings)
	{
	}
	
	///
	AmioApeSettingsDialog::~AmioApeSettingsDialog()
	{
	}
		
	///
	bool AmioApeSettingsDialog::RunModal(PlatformWindow inPlatformWindow)
	{
		int compressionLevel = mSettings.GetCompressionLevelIndex();
		if (DisplaySettingsDialog(inPlatformWindow, compressionLevel))
		{
			mSettings.SetCompressionLevel(AmioApePrivateSettings::GetCompressionLevel(
				static_cast<AmioApePrivateSettings::CompressionIndex>(compressionLevel)));
			return true;
		}
		return false;
	}
} // namespace amio