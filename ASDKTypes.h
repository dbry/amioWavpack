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
#ifndef AUDITIONSDK_ASDKTYPES_H
#define AUDITIONSDK_ASDKTYPES_H

// OS defines

#if defined(_WIN32) || defined(_WIN64)
	#define ASDK_OS_WIN
#elif defined(__MWERKS__) || defined(__GNUC__)
	#define ASDK_OS_MAC
#include <stdint.h>
#else
	#error "unrecognized OS platform"
#endif


namespace asdk
{
	typedef char				int8;
	typedef unsigned char		uint8;
	typedef short int			int16;
	typedef unsigned short int	uint16;
#if defined(ASDK_OS_WIN)
	#if defined(ASDK_MATCH_MAC_STDINT_ON_WINDOWS)
		typedef int					int32;
		typedef unsigned int		uint32;
	#else
		typedef long int			int32;
		typedef unsigned long int	uint32;
	#endif
	typedef __int64				int64;	
#elif defined(ASDK_OS_MAC)
	typedef int					int32;
	typedef unsigned int		uint32;
	typedef ::int64_t			int64;
#endif            
	typedef float				float32;
	typedef double				float64;
	typedef uint16				uniChar16;

}	//namespace asdk

#endif // AUDITIONSDK_ASDKTYPES_H
 

