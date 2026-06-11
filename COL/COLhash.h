#ifndef __COL_HASH_H__
#define __COL_HASH_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLhash
//
// Description:
//
// The default hash functions. It is a template specialized to allow easy specification
// for our simple types.
//
// This is an example of using it:
//
// COLhashmap<COLstring, COLstring> StudentTable(COLhash<COLstring>::defaultHash);
//
// Of course if you need more specialized hash function you can always write them
// and use COLhashmap independently of COLhash.
//
// Author: Nasron Cheong
// Date:   March 2004 
//
//---------------------------------------------------------------------------
#include "COLminimumInclude.h"

class COLstring;

//supported default hash implementations
//
// COLuint32/int32
// COLuint64/int64
// COLuint16/int16
// COLuint8/int8  <- this usually is unsigned char/char
// COLstring

template <class T>
struct COLhash{
   static COLindex defaultHash(const T& Key);
   //default compare defers the the types equal operator
   static bool equalKey(const T& Key1, const T& Key2){
      return (Key1 == Key2) != 0;
   }
};

template <> COLindex COLhash<COLuint64>::defaultHash(const COLuint64& Key);
template <> COLindex COLhash<COLint64>:: defaultHash(const COLint64&  Key);
template <> COLindex COLhash<COLuint32>::defaultHash(const COLuint32& Key);
template <> COLindex COLhash<COLint32>:: defaultHash(const COLint32&  Key);
template <> COLindex COLhash<COLuint16>::defaultHash(const COLuint16& Key);
template <> COLindex COLhash<COLint16>:: defaultHash(const COLint16&  Key);
template <> COLindex COLhash<COLuint8>:: defaultHash(const COLuint8&  Key);
template <> COLindex COLhash<COLint8>::  defaultHash(const COLint8&   Key);
template <> COLindex COLhash<COLstring>::defaultHash(const COLstring& Key);

template <typename T>
struct COLhashPointer{
   static COLindex defaultHash(const T& Key){
      return COLhash<COLuint64>::defaultHash((COLuint64)Key);
   }

   static bool equalKey(const T& Key1, const T& Key2){
      return (Key1 == Key2) != 0;
   }
};

#endif // end of defensive include
