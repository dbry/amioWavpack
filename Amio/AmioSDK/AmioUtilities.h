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

namespace amio
{
namespace utils
{

// String utilties
amio::UTF16String AsciiToUTF16(char const* asciiZ);

amio::UTF8String UTF16StringtoUTF8String(const amio::UTF16String& inString);

// File utilities
bool FileExists(const UTF16String& inFileName);


} // namespace utils
} // namespace amio