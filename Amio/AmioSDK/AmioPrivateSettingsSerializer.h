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

#ifndef AMIO_AMIO_PRIVATE_SETTINGS_SERIALIZER_H
#define AMIO_AMIO_PRIVATE_SETTINGS_SERIALIZER_H

#include "AmioSDK.h"
#include <map>

namespace amio 
{
	class AmioPrivateSettingsSerializer
	{		
	public:
		typedef std::pair<amio::UTF16String, asdk::int32> IntegerAttributePair;
		typedef std::map<amio::UTF16String, asdk::int32> IntegerAttributes;
		typedef std::pair<amio::UTF16String, amio::UTF16String> StringAttributePair;
		typedef std::map<amio::UTF16String, amio::UTF16String> StringAttributes;

		AmioPrivateSettingsSerializer();
		~AmioPrivateSettingsSerializer();

		/// Serialize the given attributes, returning the result in the given string.
		static void Serialize(const IntegerAttributes& inAttributes, amio::UTF16String& outString);

		/// Serialize the given attributes, returning the result in the given string.
		static void Serialize(const StringAttributes& inAttributes, amio::UTF16String& outString);

		/// Add an attribute to the list to be serialized, and then call GetSerializedString.
		void AddAttribute(const amio::UTF16String& inKey, asdk::int32 inValue);

		/// Add an attribute to the list to be serialized, and then call GetSerializedString.
		void AddAttribute(const amio::UTF16String& inKey, const amio::UTF16String& inString);

		/// Get the serialized string, after calling AddAttribute.
		amio::UTF16String GetSerializedString() const;

		/// Deserialize the given attributes.
		void Initialize(const amio::UTF16String& inString);

		/// Get the given attribute and return the default value if not found.
		asdk::int32 GetAttribute(const amio::UTF16String& inKey, asdk::int32 inDefaultValue) const;

		/// Get the given attribute and return the string value.
		amio::UTF16String GetAttribute(const amio::UTF16String& inKey) const;

		/// Get the given attribute.  Returns false if not available.
		bool QueryAttribute(const amio::UTF16String& inKey, asdk::int32& outValue) const;

	protected:
		IntegerAttributes	mAttributes;
		StringAttributes	mStringAttributes;
	};
} // namespace amio

#endif AMIO_AMIO_PRIVATE_SETTINGS_SERIALIZER_H