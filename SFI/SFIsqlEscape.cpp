//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIsqlEscape
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tuesday, August 31st, 2010 @ 03:31:47 PM
//
//---------------------------------------------------------------------------
#include "SFIprecomp.h"
#pragma hdrstop

#include "SFIsqlEscape.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

int SFIsqlEscapedSize(const COLstring& Value){
   const char* pIndex  = Value.c_str();
   int EscapedSize = Value.size();
   const char* pEnd = pIndex + EscapedSize;
   
   for (;pIndex != pEnd;pIndex++){
      switch (*pIndex){
      case '\"':
      case '\\':
      case '\'':
         EscapedSize++;
      // do nothing
      };
   }
   return EscapedSize;
}
/* SQL SERVER version
void SFIsqlEscapeString(const COLstring& Orig, COLstring* pEscaped, int NewSize){
   COLASSERT(pEscaped->size() == NewSize);
   const char* pIn  = Orig.c_str();
   char* pOut = pEscaped->get_buffer();
   const char* pEnd = pIn + Orig.size();

   for(;pIn != pEnd; pIn++){
      switch(*pIn){
      case '\'':
         *pOut ='\'';
         pOut++;
      default:
         *pOut = *pIn;
         pOut++;
      }
   }
}*/

void SFIsqlEscapeString(const COLstring& Orig, COLstring* pEscaped, int NewSize){
   COLASSERT(pEscaped->size() == NewSize);
   const char* pIn  = Orig.c_str();
   char* pOut = pEscaped->get_buffer();
   const char* pEnd = pIn + Orig.size();

   for(;pIn != pEnd; pIn++){
      switch(*pIn){
      case '\"':
      case '\'':
      case '\\':
         *pOut = '\\';
         pOut++;
      default:
         *pOut = *pIn;
         pOut++;
      }
   }
}
