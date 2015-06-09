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
#ifndef ADOBE_AUDITION_ASDK_INTERFACE_H
#define ADOBE_AUDITION_ASDK_INTERFACE_H

#include "AuditionSDK/ASDKTypes.h"

namespace asdk
{

	struct InterfaceBase
	{
		virtual void Retain() = 0;
		virtual void Release() = 0;
	protected:
		virtual ~InterfaceBase() {}
	}; // InterfaceBase

	template<class T>
	class InterfacePtr
	{
	public:
		InterfacePtr() : mPointee(0) {}
		InterfacePtr(T* inPointee) : mPointee(inPointee) { if (mPointee) mPointee->Retain(); }
		InterfacePtr(const InterfacePtr<T>& inOther) : mPointee(inOther.mPointee) { if (mPointee) mPointee->Retain(); }
		~InterfacePtr() { if (mPointee) mPointee->Release(); }

		void Swap(InterfacePtr<T>& inOther) { T* temp = mPointee; mPointee = inOther.mPointee; inOther.mPointee = temp; }

		T* Get() const { return mPointee; }
		T* operator->() const { return mPointee; }
		T& operator*() const { return *mPointee; }

		operator bool() const { return mPointee != 0; }
		
		InterfacePtr<T>& operator=(const InterfacePtr<T>& inOther) { InterfacePtr<T>(inOther).Swap(*this); return *this; }
		InterfacePtr<T>& operator=(T* inPointee) { InterfacePtr<T>(inPointee).Swap(*this); return *this; }
		
	private:
		T*		mPointee;
	}; // InterfacePtr


	class ReferenceCount
	{
	public:
		ReferenceCount() : mValue(0) {}
		void Inc() { ++mValue; }
		void Dec()  { --mValue; }
		int32 GetValue() { return mValue; }
	private:
		int32	mValue;
	}; // ReferenceCount

	
#define ASDK_CLASS_IMPL(classname) \
	public: \
		virtual void Retain() \
		{ \
			mReferenceCount.Inc(); \
		} \
		virtual void Release() \
		{ \
			mReferenceCount.Dec(); \
			if (mReferenceCount.GetValue() <= 0) \
				delete this; \
		} \
	protected: \
		asdk::ReferenceCount	mReferenceCount;
		
}	//namespace asdk

#endif //ADOBE_AUDITION_ASDK_INTERFACE_H
 

