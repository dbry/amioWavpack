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

#include "AmioSDK.h"

#ifdef _WIN32
	typedef unsigned __int32    uint32;
	typedef __int32             int32;
	typedef unsigned __int16    uint16;
	typedef __int16             int16;
	typedef unsigned __int8     uint8;
	typedef __int8              int8;
#else
	typedef asdk::uint32		uint32;
	typedef asdk::int32			int32;
	typedef asdk::uint16		uint16;
	typedef asdk::int16			int16;
	typedef asdk::uint8			uint8;
	typedef asdk::int8			int8;
#endif

typedef char				str_ansi;
typedef unsigned char		str_utf8;
typedef uint16              str_utf16;

