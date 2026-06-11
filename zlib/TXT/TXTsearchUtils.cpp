//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TXTsearchUtils
//
// Description:
//
// Implementation
//
// Author: Eric Mulvaney
// Date:   Wednesday, April 29th, 2009 @ 11:43:18 AM
//
//---------------------------------------------------------------------------
#include "TXTprecomp.h"
#pragma hdrstop

#include "TXTsearchUtils.h"

// Helper function for DWBcaseInsensitiveSearch
inline int TXTlettersEqual(int x, int y)  // ints are faster than chars.
{
   if( x == y )
   {
      return true;
   }
   else if( (x^y) == 0x20 )  // Might be the same letter, different cases.
   {
      x |= 0x20;
      return (x >= 'a') & (x <= 'z');  // && would be slower.
   }

   return false;
}

const char* TXTcaseInsensitiveSearch(const char* pHaystack, const char* pNeedle)
{
   // TODO:  I'm sure strcasestr() is faster than this, so we should
   //        use it when the system supports it.

   // Best to make sure our math is correct.
   //
   COL_STATIC_ASSERT( ('A' | 0x20) == 'a' );
   COL_STATIC_ASSERT( ('Z' | 0x20) == 'z' );

   const char* pCurrentHaystack = pHaystack;
   const char* pCurrentNeedle   = pNeedle;

   int CurrentHaystackChar = *pCurrentHaystack;
   int CurrentNeedleChar   = *pCurrentNeedle;

   for(;;)
   {
      if( !CurrentNeedleChar )
      {
         return pHaystack;
      }
      else if( !CurrentHaystackChar )
      {
         return NULL;
      }
      else if( TXTlettersEqual(CurrentHaystackChar, CurrentNeedleChar) )
      {
         CurrentHaystackChar = *++pCurrentHaystack;
         CurrentNeedleChar   = *++pCurrentNeedle;
      }
      else
      {
         CurrentHaystackChar = *(pCurrentHaystack = ++pHaystack);
         CurrentNeedleChar   = *(pCurrentNeedle   =   pNeedle);
      }
   }
}
