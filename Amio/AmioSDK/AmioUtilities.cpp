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
#include "AmioUtilities.h"
#ifdef AMIO_OS_WIN
#include <windows.h>
#elif defined (AMIO_OS_MAC)
#include <CoreServices/CoreServices.h>
#else
#error "unrecognized OS platform"
#endif

namespace {

	const asdk::uint32 kUNI_SUR_HIGH_START = 0x0000D800;
	const asdk::uint32 kUNI_SUR_HIGH_END = 0x0000DBFF;
	const asdk::uint32 kUNI_SUR_LOW_START = 0x0000DC00;
	const asdk::uint32 kUNI_SUR_LOW_END = 0x0000DFFF;
	const asdk::uint32 kUNI_REPLACEMENT_CHAR = 0x0000FFFD;
	const int kHalfShift = 10;
	const asdk::uint32 kHalfBase = 0x00010000;
	const asdk::uint32 kHalfMask = 0x000003FF;
	const amio::UTF8Char kFirstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

	
	void UTF16StringtoUTF8String_internal(
		const amio::UTF16Char* inString,
		size_t inLength,
		amio::UTF8String& outString)
	{
        if (inLength == 0 || inString == 0)
        {
            return;
        }

        size_t length = 0;
		bool singleByteChars = true;
		//	If the incoming string is a known length less tham 4 characters,
		//	then drop straight into the lower loop.
		if (inLength >= 0 && inLength < 4)
		{
			singleByteChars = false;
			length = inLength;
		}
		else 
		{
			//	If there are more than 4 characters, find out the length of the string
			//	While we are at it, we also test to see if this string is all single
			//	byte characters.
			while (inString[length] != 0)
			{
				if (inString[length] >= 0x80UL)
				{
					singleByteChars = false;
				}
				++length;
			}
		}
				
		outString.resize(length);

		//	If we are all single bytes, then we can just cast them into the output string and we are done
		if (singleByteChars)
		{
			if (length > 0)
			{
				for (unsigned char* outBufferPtr = &outString[0]; length > 0; --length)
				{
					*outBufferPtr++ = static_cast<unsigned char>(*inString++);
				}
			}
		}
		//	We either have a very short string, or some characters >= 0x80. So we run the main loop
		//	which covers all possible conversions.
		else
		{		
			size_t count = 0;
			asdk::uint32 ch;
			while ((ch = *inString++) != 0)
			{
				if (ch < 0x80UL)
				{
					outString[count++] = static_cast<amio::UTF8Char>(ch);
				}
				else
				{
					size_t bytesToWrite = 0;
					const asdk::uint32 byteMask = 0xBF;
					const asdk::uint32 byteMark = 0x80; 
					/* If we have a surrogate pair, convert to UTF32 first. */
					if (ch >= kUNI_SUR_HIGH_START && ch <= kUNI_SUR_HIGH_END && *inString != 0)
					{
						asdk::uint32 ch2 = *inString;
						if (ch2 >= kUNI_SUR_LOW_START && ch2 <= kUNI_SUR_LOW_END)
						{
							ch = ((ch - kUNI_SUR_HIGH_START) << kHalfShift)
								+ (ch2 - kUNI_SUR_LOW_START) + kHalfBase;
							++inString;
						}
					}
					/* Figure out how many bytes the result will require */
					if (ch < (asdk::uint32)0x800)
					{
						bytesToWrite = 2;
					}
					else if (ch < (asdk::uint32)0x10000)
					{
						bytesToWrite = 3;
					}
					else if (ch < (asdk::uint32)0x200000)
					{
						bytesToWrite = 4;
					}
					else 
					{
						bytesToWrite = 2;
						ch = kUNI_REPLACEMENT_CHAR;
					}

					length += (bytesToWrite - 1);
					size_t newPosition = count + bytesToWrite;
					outString.resize(length);
					switch (bytesToWrite)
					{	/* note: everything falls through. */
						case 4:
							outString[--newPosition] = (amio::UTF8Char)((ch | byteMark) & byteMask);
							ch >>= 6;
						case 3:
							outString[--newPosition] = (amio::UTF8Char)((ch | byteMark) & byteMask);
							ch >>= 6;
						case 2:
							outString[--newPosition] = (amio::UTF8Char)((ch | byteMark) & byteMask);
							ch >>= 6;
						case 1:
							outString[--newPosition] = (amio::UTF8Char)(ch | kFirstByteMark[bytesToWrite]);
					}
					count += bytesToWrite;
				}
			}
		}
	}

} // private namespace

namespace amio
{

namespace utils
{

// This routine relies on ASCII being a proper subset of Unicode, so the
// routine just rams the ASCII char into the UTF-16 bucket.
amio::UTF16String AsciiToUTF16(
	char const* asciiZ)
{
	size_t const length = std::strlen(asciiZ);

	// Use the not-oft-used string(beginInputIterator, endInputIterator) constructor.
	amio::UTF16String result(asciiZ, asciiZ + length);
	return result;
}

amio::UTF8String UTF16StringtoUTF8String(const amio::UTF16String& inString)
{
	amio::UTF8String outString;
	UTF16StringtoUTF8String_internal(inString.c_str(), inString.size(), outString);
	return outString;
}

bool FileExists(const UTF16String& inFileName)
{
#ifdef AMIO_OS_WIN

	DWORD attrs = 0;

	attrs = ::GetFileAttributesW((LPCWSTR)inFileName.c_str());

	return (attrs != 0xFFFFFFFF);

#elif defined(AMIO_OS_MAC)
	FSRef pathref;
	Boolean isDir;
	
	amio::UTF8String utf8Path = UTF16StringtoUTF8String(inFileName);
	OSStatus stat = ::FSPathMakeRef((const unsigned char*)utf8Path.c_str(), &pathref, &isDir);
	return (noErr == stat);
	
#else
	#error "unrecognized OS platform"
#endif

}

} // namespace utils
} // namespace amio

