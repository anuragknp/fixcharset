/**************************************************************
File			:
Description		:
Date Created	:
Created By		:
Last Modified	:
Last Reviewed	:
 **************************************************************/

#ifndef _MB_YDEFINES_H
#define _MB_YDEFINES_H

//#include "yfwddecl.h"

#ifdef _WIN32 // WIN32 specific definitions

typedef unsigned __int64 yUInt64;
typedef __int64 yInt64;

typedef unsigned __int32 yUInt32;
typedef __int32 yInt32;

typedef unsigned __int16 yUInt16;
typedef __int16 yInt16;

typedef struct _stat yFileStat;

#pragma warning(disable:4127)
#pragma warning(disable:4706)

#pragma warning(disable:4511)	// copy constructor could not be generated
#pragma warning(disable:4512)	// assignment operator could not be generated
#pragma warning(disable:4244)	// conversion from 'int' to 'yChar', possible loss of data
#pragma warning(disable:4267)	// conversion from 'size_t' to 'int', possible loss of data
#pragma warning(disable:4706)	// assignment within conditional expression
#pragma warning(disable:4018)	// '>=' : signed/unsigned mismatch
#pragma warning(disable:4530) //C++ exception handler used, but unwind semantics are not enabled

#pragma warning(disable:4996)	// warning C4996: 'strnicmp': 
// The POSIX name for this item is deprecated. Instead, use the ISO C++ 
//conformant name: _strnicmp. See online help for details.
#pragma warning ( disable : 4251 ) 

#define TRUE 1
#define FALSE 0

#define UNREFERENCED_PARAMETER(P) (P)

// #elif defined(_UNIX)			// UNIX specific definitions
#else
#include <inttypes.h>

#define UNREFERENCED_PARAMETER(p) p
typedef uint64_t	            yUInt64;
typedef int64_t	                yInt64;
typedef uint32_t yUInt32;
typedef int32_t yInt32;

typedef uint16_t yUInt16;
typedef int16_t yInt16;

typedef int ySocket;
typedef struct stat yFileStat;

#define closesocket(x)		close(x)
#endif //_WIN32

// COMMON definitions
#define YNULCHAR '\0'
#define YZERO 0

#define WY_YES	1
#define WY_NO	0

// Common Type Definitions
typedef bool yBool;
typedef unsigned char yByte;
typedef const unsigned char yCByte;
typedef unsigned char const yByteC;
typedef char yChar;
typedef const char yCChar;
typedef unsigned char yUChar;
typedef wchar_t yWChar;
typedef const wchar_t yCWChar;
typedef long yLong;
typedef unsigned long yULong;

typedef long long int yLLInt;

class wyString;

#ifdef _DEBUG // Definitions for _DEBUG mode
#ifndef ASSERT
#define ASSERT(x)			assert(x)
#endif
#ifndef VERIFY
#define VERIFY(x)			assert(x)
#endif
#define VERIFY_VAL(x,y)		assert((x)==(y))
#define VERIFY_NOTVAL(x,y)	assert((x)!=(y))

#else		// Definitions for NON _DEBUG mode (release)

#ifndef ASSERT
#define ASSERT(x)			((void)0)
#endif
#ifndef VERIFY
#define VERIFY(x)			(x)
#endif
#define VERIFY_VAL(x,y)		(x)
#define VERIFY_NOTVAL(x,y)	(x)

#endif		// #ifdef _DEBUG

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

#ifndef TRUE
#define TRUE 1
#endif  // TRUE

#ifndef FALSE
#define FALSE 0
#endif	//FALSE

#ifdef _WIN32
typedef yUInt32 yCodePage;
#define WY_CODEPAGE_ASCII CP_ACP
#define WY_CODEPAGE_UTF8 CP_UTF8
#else
typedef yCChar* yCodePage;
#define WY_CODEPAGE_ASCII "en_US.iso88591"
#define WY_CODEPAGE_UTF8 "en_US.utf8"
#endif

#endif		// #ifndef _DEFINES_H