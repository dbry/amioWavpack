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

#include "AmioWavpackSettingsDialog.h"
#include "AmioModule.h"
#include "AmioUtilities.h"
#include "resource.h"

namespace
{
	int gRadioIndexCompressionLevel = 0;

	void CenterDialog(HWND hDlg)
	{
		int  nHeight;
		int  nWidth;
		int  nTitleBits;
		RECT rcDialog;
		RECT rcParent;
		int  xOrigin;
		int  yOrigin;
		HWND hParent = GetParent(hDlg);

		if  (hParent == NULL)
			hParent = GetDesktopWindow();

		GetClientRect(hParent, &rcParent);
		ClientToScreen(hParent, (LPPOINT)&rcParent.left);  // point(left,  top)
		ClientToScreen(hParent, (LPPOINT)&rcParent.right); // point(right, bottom)

		// Center on Title: title bar has system menu, minimize,  maximize bitmaps
		// Width of title bar bitmaps - assumes 3 of them and dialog has a sysmenu
		nTitleBits = GetSystemMetrics(SM_CXSIZE);

		// If dialog has no sys menu compensate for odd# bitmaps by sub 1 bitwidth
		if  ( ! (GetWindowLong(hDlg, GWL_STYLE) & WS_SYSMENU))
			nTitleBits -= nTitleBits / 3;

		GetWindowRect(hDlg, &rcDialog);
		nWidth  = rcDialog.right  - rcDialog.left;
		nHeight = rcDialog.bottom - rcDialog.top;

		xOrigin = max(rcParent.right - rcParent.left - nWidth, 0) / 2
				+ rcParent.left - nTitleBits;

		yOrigin = max(rcParent.bottom - rcParent.top - nHeight, 0) / 3
				+ rcParent.top;

		SetWindowPos(hDlg, NULL, xOrigin, yOrigin, nWidth, nHeight, SWP_NOZORDER);
	}

	INT_PTR WINAPI WavpackSettingsDialogProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (wMsg)
		{
		case  WM_INITDIALOG:
		{
			CenterDialog(hDlg);
			
			int radioToCheck = IDC_RADIO2;
			switch (gRadioIndexCompressionLevel)
			{
			case 0:
				radioToCheck = IDC_RADIO1;
				break;
			default:
			case 1:
				radioToCheck = IDC_RADIO2;
				break;
			case 2:
				radioToCheck = IDC_RADIO3;
				break;
			case 3:
				radioToCheck = IDC_RADIO4;
				break;
//			case 4:
//				radioToCheck = IDC_RADIO5;
//				break;
			}
			CheckRadioButton(hDlg, IDC_RADIO1, IDC_RADIO4, radioToCheck);
			return TRUE;
		}

		case  WM_COMMAND:
		{
	  		short item = (short) (wParam & 0x0000FFFF);
			short cmd = (short) (wParam >> 16);

			switch  (item)
			{
			case IDOK:
				if (cmd == BN_CLICKED)
				{
					if (IsDlgButtonChecked(hDlg, IDC_RADIO1))
						gRadioIndexCompressionLevel = 0;
					else if (IsDlgButtonChecked(hDlg, IDC_RADIO2))
						gRadioIndexCompressionLevel = 1;
					else if (IsDlgButtonChecked(hDlg, IDC_RADIO3))
						gRadioIndexCompressionLevel = 2;
					else if (IsDlgButtonChecked(hDlg, IDC_RADIO4))
						gRadioIndexCompressionLevel = 3;
//					else if (IsDlgButtonChecked(hDlg, IDC_RADIO5))
//						gRadioIndexCompressionLevel = 4;
					EndDialog(hDlg, item);
					return TRUE;
				}
			case IDCANCEL:
				if (cmd == BN_CLICKED)
				{
					EndDialog(hDlg, item);
					return TRUE;
				}
			}
		}

		}
		return FALSE;
	}

}

namespace amio
{

	//////////////////////////////////////////////////////
	///
	AmioWavpackSettingsDialog::AmioWavpackSettingsDialog(AmioWavpackPrivateSettings& inSettings)
		: mSettings(inSettings)
	{
	}

	//////////////////////////////////////////////////////
	///
	AmioWavpackSettingsDialog::~AmioWavpackSettingsDialog()
	{
	}

	//////////////////////////////////////////////////////
	///
	bool AmioWavpackSettingsDialog::RunModal(PlatformWindow inPlatformWindow)
	{
		gRadioIndexCompressionLevel = mSettings.GetCompressionLevelIndex();
		INT_PTR nResult = DialogBoxParam(amio::gModuleInstance,
								amio::utils::AsciiToUTF16("IDD_DIALOG_SETTINGS").c_str(),
								inPlatformWindow,
								WavpackSettingsDialogProc,
								NULL);
		if (IDOK == nResult)
		{
			mSettings.SetCompressionLevel(
					AmioWavpackPrivateSettings::GetCompressionLevel(
					static_cast<AmioWavpackPrivateSettings::CompressionIndex>(gRadioIndexCompressionLevel)));
			return true;
		}
		return false;
	}

} // namespace amio
