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
#include <commctrl.h>

namespace
{
	amio::AmioWavpackPrivateSettings *pSettings = NULL;

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
		amio::AmioWavpackPrivateSettings& mSettings = *pSettings;

		switch (wMsg)
		{
		case  WM_INITDIALOG:
		{
			int mode = mSettings.GetCompressionMode();
			int hybrid_mode = (mode % 1000) - (mode % 100);
			int wvc_mode = (mode % 100) - (mode % 10);
			CenterDialog(hDlg);
			
            EnableWindow (GetDlgItem (hDlg, IDC_BITRATE), hybrid_mode);
            EnableWindow (GetDlgItem (hDlg, IDC_BITRATE_TEXT), hybrid_mode);
            EnableWindow (GetDlgItem (hDlg, IDC_CORRECTION), hybrid_mode);

			int radioToCheck = IDC_LOSSLESS;
			switch (mode / 1000)
			{
			case 1:
				radioToCheck = hybrid_mode ? IDC_HYBRID_FAST : IDC_LOSSLESS_FAST;
				break;
			default:
			case 2:
				radioToCheck = hybrid_mode ? IDC_HYBRID : IDC_LOSSLESS;
				break;
			case 3:
				radioToCheck = hybrid_mode ? IDC_HYBRID_HIGH : IDC_LOSSLESS_HIGH;
				break;
			case 4:
				radioToCheck = hybrid_mode ? IDC_HYBRID_VHIGH : IDC_LOSSLESS_VHIGH;
				break;
			}

			CheckRadioButton(hDlg, IDC_LOSSLESS, IDC_HYBRID_FAST, radioToCheck);
            CheckDlgButton (hDlg, IDC_CORRECTION, wvc_mode);

            SendDlgItemMessage (hDlg, IDC_EXTRA_SLIDER, TBM_SETRANGE, 0, MAKELONG (0, 6));
            SendDlgItemMessage (hDlg, IDC_EXTRA_SLIDER, TBM_SETPOS, 1, mode % 10);

            SendDlgItemMessage (hDlg, IDC_BITRATE, CB_LIMITTEXT, 5, 0);

			char str [16];
			int br_max = mSettings.GetMaximumBitrate();
            for (int i = mSettings.nearestStandardBitrate(mSettings.GetMinimumBitrate()); i <= br_max; ++i)
                if (mSettings.isStandardBitrate (i)) {
                    sprintf (str, "%d", i);
                    SendDlgItemMessageA (hDlg, IDC_BITRATE, CB_ADDSTRING, 0, (long) str);
                }

            sprintf (str, "%d", mSettings.GetCurrentBitrate());
            SetDlgItemTextA (hDlg, IDC_BITRATE, str);

            EnableWindow (GetDlgItem (hDlg, IDC_WRITETAGS), mSettings.GetAppendApeTagsMode());
            CheckDlgButton (hDlg, IDC_WRITETAGS, mSettings.GetAppendApeTagsMode() & 1);

            SetFocus (GetDlgItem (hDlg, radioToCheck));
			return TRUE;
		}

		case  WM_COMMAND:
		{
	  		short item = (short) (wParam & 0x0000FFFF);
			short cmd = (short) (wParam >> 16);

			switch  (item)
			{
			case IDC_LOSSLESS: case IDC_LOSSLESS_HIGH: case IDC_LOSSLESS_VHIGH: case IDC_LOSSLESS_FAST:
				if (cmd == BN_CLICKED)
				{
                    EnableWindow (GetDlgItem (hDlg, IDC_BITRATE), FALSE);
                    EnableWindow (GetDlgItem (hDlg, IDC_BITRATE_TEXT), FALSE);
                    EnableWindow (GetDlgItem (hDlg, IDC_CORRECTION), FALSE);
                    break;
				}
			case IDC_HYBRID: case IDC_HYBRID_HIGH: case IDC_HYBRID_VHIGH: case IDC_HYBRID_FAST:
				if (cmd == BN_CLICKED)
				{
                    EnableWindow (GetDlgItem (hDlg, IDC_BITRATE), TRUE);
                    EnableWindow (GetDlgItem (hDlg, IDC_BITRATE_TEXT), TRUE);
                    EnableWindow (GetDlgItem (hDlg, IDC_CORRECTION), TRUE);
                    break;
				}
			case IDOK:
				if (cmd == BN_CLICKED)
				{
					int mode;

					if (IsDlgButtonChecked(hDlg, IDC_LOSSLESS_FAST))
						mode = 1000;
					else if (IsDlgButtonChecked(hDlg, IDC_LOSSLESS))
						mode = 2000;
					else if (IsDlgButtonChecked(hDlg, IDC_LOSSLESS_HIGH))
						mode = 3000;
					else if (IsDlgButtonChecked(hDlg, IDC_LOSSLESS_VHIGH))
						mode = 4000;
					else if (IsDlgButtonChecked(hDlg, IDC_HYBRID_FAST))
						mode = 1100;
					else if (IsDlgButtonChecked(hDlg, IDC_HYBRID))
						mode = 2100;
					else if (IsDlgButtonChecked(hDlg, IDC_HYBRID_HIGH))
						mode = 3100;
					else if (IsDlgButtonChecked(hDlg, IDC_HYBRID_VHIGH))
						mode = 4100;

					if (IsDlgButtonChecked(hDlg, IDC_CORRECTION))
						mode += 10;

                    int i = (int) SendDlgItemMessage (hDlg, IDC_EXTRA_SLIDER, TBM_GETPOS, 0, 0);

                    if (i >= 0 && i <= 6)
						mode += i;

					mSettings.SetCompressionMode (mode);

					char str [16];
                    GetWindowTextA (GetDlgItem (hDlg, IDC_BITRATE), str, sizeof (str));
					int new_br = atol (str);

                    if (new_br && new_br != mSettings.GetCurrentBitrate() && new_br <= 99999)
						mSettings.SetCurrentBitrate (new_br < mSettings.GetMinimumBitrate() ? mSettings.GetMinimumBitrate() : new_br);

					if (mSettings.GetAppendApeTagsMode())		// don't change this setting unless there are tags
						mSettings.SetAppendApeTagsMode (IsDlgButtonChecked(hDlg, IDC_WRITETAGS) ? 1 : 2);

					EndDialog(hDlg, item);
					return TRUE;
				}
			case IDCANCEL:
				if (cmd == BN_CLICKED)
				{
					EndDialog(hDlg, item);
					return TRUE;
				}
			case IDABOUT:
				if (cmd == BN_CLICKED)
				{
					amio::UTF16String about(L"AmioWavpack Plugin Version 2.0a\nWavPack Library Version ");
					about += mSettings.GetWavpackLibraryVersion();
					about += L"\nCopyright (c) 2016 David Bryant";
					MessageBox (hDlg, about.c_str(), L"About AmioWavpack Plugin", MB_OK);
					break;
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
		pSettings = &mSettings;
		INT_PTR nResult = DialogBoxParam(amio::gModuleInstance,
								amio::utils::AsciiToUTF16("IDD_DIALOG_SETTINGS").c_str(),
								inPlatformWindow,
								WavpackSettingsDialogProc,
								NULL);
		if (IDOK == nResult)
		{
			return true;
		}
		return false;
	}

} // namespace amio
