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

#include "AmioModule.h"

namespace amio
{
HINSTANCE gModuleInstance = NULL;
} // namespace amio

BOOL APIENTRY DllMain(HINSTANCE		hModule, 
					  DWORD			inReason, 
					  LPVOID		lpReserved)
{
	switch (inReason)
	{
		case DLL_PROCESS_ATTACH:
			amio::gModuleInstance = hModule;
			break;

		case DLL_THREAD_ATTACH:
			break;

		case DLL_THREAD_DETACH:
			break;

		case DLL_PROCESS_DETACH:
			amio::gModuleInstance = NULL;
			break;
	}
	return TRUE;
}

