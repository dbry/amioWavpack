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

#ifndef AMIO_AMIOAPE_SETTINGS_DIALOG_H
#define AMIO_AMIOAPE_SETTINGS_DIALOG_H

#include "AmioApePrivateSettings.h"
#ifdef AMIO_OS_WIN
#include "windows.h"
#endif
namespace amio 
{
	class AmioApeSettingsDialog
	{
	public:
		AmioApeSettingsDialog(AmioApePrivateSettings& inSettings);
		~AmioApeSettingsDialog();

#ifdef AMIO_OS_WIN
		typedef HWND PlatformWindow;
#elif defined(AMIO_OS_MAC)
		typedef void* PlatformWindow;		
#else
#error "unrecognized OS platform"
#endif		
		bool RunModal(PlatformWindow inPlatformWindow);
			
	private:
		AmioApePrivateSettings&						mSettings;
	};
} // namespace amio

#endif AMIO_AMIOAPE_SETTINGS_DIALOG_H