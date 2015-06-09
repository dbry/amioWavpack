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

// Amio_PlugIns
#include "AmioPrivateSettingsSerializer.h"
#include "AmioUtilities.h"

#include <string>

namespace
{
	bool GetValueAsInteger(const amio::UTF16String& inValue, asdk::int32& outValue)
	{
		amio::UTF8String num_text8 = amio::utils::UTF16StringtoUTF8String(inValue);

		outValue = atoi(reinterpret_cast<const char*>(num_text8.c_str()));
		return true;
	}
} // private namespace

namespace amio 
{

	////////////////////////////////////////////////////////////////////////////
	AmioPrivateSettingsSerializer::AmioPrivateSettingsSerializer()
	{
	}

	////////////////////////////////////////////////////////////////////////////
	AmioPrivateSettingsSerializer::~AmioPrivateSettingsSerializer()
	{
	}

	////////////////////////////////////////////////////////////////////////////
	void AmioPrivateSettingsSerializer::Serialize(const IntegerAttributes& inAttributes, amio::UTF16String& outString)
	{
		outString.clear();
		for (IntegerAttributes::const_iterator it = inAttributes.begin(); it != inAttributes.end(); ++it)
		{
			char numberBuffer[256];
			sprintf(numberBuffer, "%ld", (*it).second);
			outString += (*it).first + amio::utils::AsciiToUTF16(":0:") + 
				amio::utils::AsciiToUTF16(numberBuffer) + amio::utils::AsciiToUTF16(";");
		}
	}

	////////////////////////////////////////////////////////////////////////////
	void AmioPrivateSettingsSerializer::Serialize(const StringAttributes& inAttributes, amio::UTF16String& outString)
	{
		outString.clear();
		for (StringAttributes::const_iterator it = inAttributes.begin(); it != inAttributes.end(); ++it)
		{
			outString += (*it).first + amio::utils::AsciiToUTF16(":1:") + (*it).second + amio::utils::AsciiToUTF16(";");
		}
	}
	////////////////////////////////////////////////////////////////////////////
	void AmioPrivateSettingsSerializer::AddAttribute(const amio::UTF16String& inKey, asdk::int32 inValue)
	{
		mAttributes.insert(IntegerAttributePair(inKey, inValue));
	}

	////////////////////////////////////////////////////////////////////////////
	void AmioPrivateSettingsSerializer::AddAttribute(const amio::UTF16String& inKey, const amio::UTF16String& inString)
	{
		mStringAttributes.insert(StringAttributePair(inKey, inString));
	}

	////////////////////////////////////////////////////////////////////////////
	amio::UTF16String AmioPrivateSettingsSerializer::GetSerializedString() const
	{
		amio::UTF16String result;
		amio::UTF16String resultString;
		Serialize(mAttributes, result);
		Serialize(mStringAttributes, resultString);
		return result+resultString;
	}

	////////////////////////////////////////////////////////////////////////////
	void AmioPrivateSettingsSerializer::Initialize(const amio::UTF16String& inString)
	{
		mAttributes.clear();

		amio::UTF16String searchCharacters = amio::utils::AsciiToUTF16(":;");
		amio::UTF16String integerCharacters = amio::utils::AsciiToUTF16("-0123456789");

		amio::UTF16String::size_type startIndex = 0;		
		amio::UTF16String::size_type keyEndIndex;
		amio::UTF16String::size_type typeEndIndex;
		amio::UTF16String::size_type valueEndIndex;
		while(1)
		{
			if ((keyEndIndex = inString.find(searchCharacters[0], startIndex)) == amio::UTF16String::npos)
			{
				break;
			}
			if ((typeEndIndex = inString.find(searchCharacters[0], keyEndIndex+1)) == amio::UTF16String::npos)
			{
				break;
			}
			if ((valueEndIndex = inString.find(searchCharacters[1], typeEndIndex)) == amio::UTF16String::npos)
			{
				break;
			}

			amio::UTF16String::size_type keyStartIndex = startIndex;
			startIndex = valueEndIndex + 1; 

			amio::UTF16String str;
			// Try to get an value.
			if (valueEndIndex <= typeEndIndex + 1)
			{
				continue;
			}
			str = inString.substr(keyEndIndex + 1, typeEndIndex - keyEndIndex - 1);
			if (amio::UTF16String::npos != str.find_first_not_of(integerCharacters))
			{
				continue;	// This value is not an integer
			}
			asdk::int32 value = 0;
			if (!GetValueAsInteger(str, value))
			{
				continue;	// This value is not an integer
			}
			if (keyEndIndex - 1 <= keyStartIndex)
			{
				continue;	// Could not get a value string.
			}
			if (keyEndIndex - 1 <= keyStartIndex)
			{
				continue;	// Could not get a value string.
			}
			if (value == 0)	
			{
				str = inString.substr(typeEndIndex + 1, valueEndIndex - typeEndIndex - 1);
				if (amio::UTF16String::npos != str.find_first_not_of(integerCharacters))
				{
					continue;	// This value is not an integer
				}
				value = 0;
				if (!GetValueAsInteger(str, value))
				{
					continue;	// This value is not an integer
				}
				AddAttribute(inString.substr(keyStartIndex, keyEndIndex - keyStartIndex), value);
			}
			else
			{
				AddAttribute(inString.substr(keyStartIndex, keyEndIndex - keyStartIndex), inString.substr(typeEndIndex+1, valueEndIndex - typeEndIndex -1));
			}

		}
	}

	////////////////////////////////////////////////////////////////////////////
	amio::UTF16String AmioPrivateSettingsSerializer::GetAttribute(const amio::UTF16String& inKey) const
	{
		amio::UTF16String ret;
		StringAttributes::const_iterator it = mStringAttributes.find(inKey);
		if (it == mStringAttributes.end())
		{
			return ret;
		}
		return (*it).second;
	}

	////////////////////////////////////////////////////////////////////////////
	asdk::int32 AmioPrivateSettingsSerializer::GetAttribute(const amio::UTF16String& inKey, asdk::int32 inDefaultValue) const
	{
		asdk::int32 result = inDefaultValue;
		QueryAttribute(inKey, result);
		return result;
	}

	////////////////////////////////////////////////////////////////////////////
	bool AmioPrivateSettingsSerializer::QueryAttribute(const amio::UTF16String& inKey, asdk::int32& outValue) const
	{
		IntegerAttributes::const_iterator it = mAttributes.find(inKey);
		if (it == mAttributes.end())
		{
			return false;
		}
		outValue = (*it).second;
		return true;
	}

} // namespace amio
