//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAlineParser
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday, June 30th, 2010 @ 10:22:57 AM
//
//---------------------------------------------------------------------------
#include "LUAprecomp.h"
#pragma hdrstop

#include "LUAlineParser.h"

#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <ctype.h>

//  
void LUAparseLineLengths(const COLstring& Data, COLarray<int>* pLengthVector)
{
   const char* pChar = Data.c_str();
   const char* pEnd = pChar + Data.size();
   int Line = 0;
   int Length =0;
   int WhitespaceLength = 0; // length of the current chunk of whitespace

   while (pChar < pEnd)
   {
      switch(*pChar)
      {
      case '\r':
         // We ignore it - doesn't count for line length
         break;
      case '\n':
         COL_VAR3(Line, Length, WhitespaceLength);
         if (pLengthVector->size() <= Line) pLengthVector->resize(Line+1);
         Length -= WhitespaceLength;
         pLengthVector->operator [](Line) = Length;
         Line++;
         Length = 0;
         WhitespaceLength = 0;
         break;
      case '\t':{
         int LengthIncrease = 8 - (Length % 8); // CodeMirror indents to the next multiple of 8
         Length += LengthIncrease;
         WhitespaceLength += LengthIncrease;
         break;
      }
      default:
         if (COLisspace((unsigned char)*pChar)){
            WhitespaceLength++;
         } else {
            WhitespaceLength = 0;
         }
         Length++;
      }
      pChar++;
   }
   if (Length)
   {
      COL_VAR2(Line, Length);
      if (pLengthVector->size() <= Line) pLengthVector->resize(Line+1);
      pLengthVector->operator [](Line) = Length;
   }
}

int LUAmaxLineLengthInBlock(const COLarray<int>& LineLengthVector, int Start, int End)
{
   COL_FUNCTION(LUAmaxLineLengthInBlock);
   if (LineLengthVector.size() == 0 || Start >= LineLengthVector.size()) return 0;
   if  (End >= LineLengthVector.size()) End = LineLengthVector.size()-1;


   int Max =0;
   for (int i = Start; i <= End; i++)
   {
      Max = COL_MAX(Max, LineLengthVector[i]);
   }
   return Max;
}
