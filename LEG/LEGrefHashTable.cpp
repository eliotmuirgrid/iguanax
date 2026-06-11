//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LEGrefHashTable
//
// Description:
//
// Simple templated reference hash table class
//
// Author: Eliot Muir
// Date:   Tue 08/25/1998
//
//
//---------------------------------------------------------------------------
#include "LEGprecomp.h"
#pragma hdrstop

#include "LEGrefHashTable.h"
#include <COL/COLstring.h>

//colstring specialization
template<>
size_t LEGhashFunc<COLstring>(const COLstring& String)
{
   if (String.size() >= 2)
   {
      return (((((String[String.size()- 1] << 8)
              + String[String.size()-2]) << 8)
              + String[1]) << 8) + String[0];
   }
   else
   {
      return 123432;
   }
}

size_t LEGhashFuncCOLint64(const COLuint64& Value)
{
   //do a high/lo xor
   COLuint32 High,Low;
   LEGgetBitHalves<COLuint64,COLuint32>(Value,Low,High);
   COLuint32 NewValue = High ^ Low;
   return NewValue;
}

//COLuint64 specialization
template <>
size_t LEGhashFunc< COLuint64 >(const COLuint64& Value)
{
   return LEGhashFuncCOLint64(Value);
}

//COLint64 specialization
template <>
size_t LEGhashFunc< COLint64 >(const COLint64& Value)
{
   return LEGhashFuncCOLint64(Value);
}
