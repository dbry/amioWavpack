/**************************************************************************
*
* ADOBE CONFIDENTIAL
* ___________________
*
*  Copyright 2008 Adobe Systems Incorporated
*  All Rights Reserved.
*
* NOTICE:  All information contained herein is, and remains the property of
* Adobe Systems Incorporated  and its suppliers,  if any.  The intellectual 
* and technical concepts contained herein are proprietary to  Adobe Systems 
* Incorporated  and its suppliers  and may be  covered by U.S.  and Foreign 
* Patents,patents in process,and are protected by trade secret or copyright 
* law.  Dissemination of this  information or reproduction of this material
* is strictly  forbidden  unless prior written permission is  obtained from 
* Adobe Systems Incorporated.
**************************************************************************/
#ifndef ADOBE_AUDITION_ASDK_STRING_H
#define ADOBE_AUDITION_ASDK_STRING_H

#include "AuditionSDK/ASDKInterface.h"

#include <algorithm>

namespace asdk
{

	////////////////////////////////////////////////////////////////////////// IString
	///
	///
	struct IString
		: public InterfaceBase
	{
		virtual const uint32 GetLength() const = 0; ///< Returns the length of the string excluding the terminating zero.
		virtual const uniChar16* GetData() const = 0; ///< Returns pointer to zero terminated string. Do not modify or delete
		virtual void SetData(const uniChar16* inBuffer, uint32 inLength) = 0;	
	}; // IString


	typedef asdk::InterfacePtr<IString> IStringPtr;


	////////////////////////////////////////////////////////////////////////// String
	///
	/// Simple container implementing the IString interface
	///
	class String
		: public IString
	{
		ASDK_CLASS_IMPL(String)
	public:
		String()
			: mBuffer(0), mLength(0)
		{
			SetData(NULL, 0);
		}

		String(const uniChar16* inData, uint32 inLength)
			: mBuffer(0), mLength(0)
		{
			SetData(inData, inLength);
		}
		
		virtual ~String()
		{
			delete[] mBuffer;
		}

		virtual const uint32 GetLength() const
		{
			return mLength;
		}

		virtual const uniChar16* GetData() const
		{
			return mBuffer;
		}
		
		virtual void SetData(const uniChar16* inBuffer, uint32 inLength)
		{
			if (inLength != mLength || !inBuffer)
			{
				delete[] mBuffer;
				mLength = inBuffer ? inLength : 0;
				mBuffer = inBuffer ? new uniChar16[mLength + 1] : 0;
			}

			if (mBuffer)
			{
				mBuffer[mLength] = 0;
				std::copy(inBuffer, inBuffer + inLength, mBuffer);
			}
			else
			{
				// Don't leave a null buffer.  Store an empty string instead.
				mBuffer = new uniChar16[1];
				mLength = 0;
				mBuffer[mLength] = 0;
			}
		}
		
	private:
		uniChar16*	mBuffer;
		uint32		mLength;
	}; // String
	
}	//namespace asdk

#endif //ADOBE_AUDITION_ASDK_STRING_H

