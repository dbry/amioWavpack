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

#ifndef AMIO_AMIOWAVPACK_SETTINGS_DIALOG_H
#define AMIO_AMIOWAVPACK_SETTINGS_DIALOG_H

#include "AmioWavpackPrivateSettings.h"
#ifdef AMIO_OS_WIN
#include "windows.h"
#endif
namespace amio 
{
	class AmioWavpackSettingsDialog
	{
	public:
		AmioWavpackSettingsDialog(AmioWavpackPrivateSettings& inSettings);
		~AmioWavpackSettingsDialog();

#ifdef AMIO_OS_WIN
		typedef HWND PlatformWindow;
#elif defined(AMIO_OS_MAC)
		typedef void* PlatformWindow;		
#else
#error "unrecognized OS platform"
#endif		
		bool RunModal(PlatformWindow inPlatformWindow);
			
	private:
		AmioWavpackPrivateSettings&						mSettings;
	};
} // namespace amio

#endif AMIO_AMIOWAVPACK_SETTINGS_DIALOG_H
