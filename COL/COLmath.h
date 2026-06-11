#ifndef __COL_MATH_H__
#define __COL_MATH_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLmath
//
// Description:
//
// Commonly used mathematical functions - definitely take the time to read through
// this file.
//
// Author: Eliot Muir
// Date:   Fri 01/16/2004
//---------------------------------------------------------------------------
#define _CRT_RAND_S  // to enable rand_s() on Windows. Must appear before first stdlib include. Note COLerror.h also includes stdlib

#include "COLminimumInclude.h"
#include "COLerror.h"
#include <stdlib.h>
#ifdef _WIN32
#   include <mbstring.h>
#endif
#include <math.h>
// TODO - should really unit test the multibyte stuff in
// particular
// TODO consider private assignment and copy constructor

class COLmath{
public:
   static void resetRandomGenerator();
   // returns a random whole number {0, 1, 2, ...} that is less than Max
   // Max must be <= RAND_MAX on Posix or an error is thrown
   static COLuint32 randomNumber(COLuint32 Max);

   // this was wrapped because of the implementation details
   // on the various platforms we use:
   // - Windows defines this function as "_atoi4"
   // - POSIX defines it as "atoll"
   // - AIX and HPUX don't define it at all, so an implementation
   // had to be written, as portrayed below.  This home-made implementation
   // has been designed to produce the same output as the Windows and
   // POSIX APIs.  In particular, it strips leading white space and,
   // aside from a possible '-' at the beginning of the number, treats
   // any non-digit characters as string terminators.  See the unit test
   // for examples of input and output.
   static COLint64 atoi64(const char* pNumberString);

   // parses char array representation of unsigned small integer
   // using ::atoi, plus:
   // if the char array has length 0, 0 is returned.
   // if the char array is negative, 0 is returned.
   // if the char array is too big to be represented, 0 is returned
   static COLuint16 atous(const char* pNumberString);

   // parses unsigned long integers
   // if the string contains any invalid characters (anything other
   // than 0-9), 0 is returned.
   // if the integer represented by pNumberString is greater
   // than COL_UINT_64_MAX, then COL_UINT_64_MAX will be returned.
   static COLuint64 parseUint64(const char* pNumberString, COLuint32 CountOfDigit);

   static COLint64 power(const COLint64 Base, const COLint64 Exponent);

   static char toUpper(char Character);

   // IMPORTANT NOTE:
   //
   // Compile Time Macro, CHM_MULTI_BYTE_SUPPORT_OFF, introduced
   // here to allow us to turn off multi byte support.  This
   // may be necessary because, under rare circumstances on
   // some linux platforms, calls to mblen and mbrlen have
   // resulted in segmentation faults.  If turned off, then
   // the pointer is simply incremented by the size of a single
   // character.
   static void incMultiBytePointer(COLuint8*& pPointer);

   // This function is similar to incMultiBytePointer, but it only returns the
   // length of the multi-byte character without incrementing it.
   static COLuint32 multiByteLength(COLuint8*& pPointer);

   static bool isInteger(const COLstring& Integer);
   static bool isInteger(const char* pInteger);
};

#endif // end of defensive include
