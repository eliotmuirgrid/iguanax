//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IG3WEBforms
//
// Description:
//
// This class converts a string with just \0D characters to use
// complete \0D \0A sequences so that it can be output using standard
// ostream operators.
//
// Author: Eliot Muir
// Date:   Tuesday, March 21st, 2006 @ 11:42:48 AM
//
//---------------------------------------------------------------------------
#include "FMTprecomp.h"
#pragma hdrstop

#include <COL/COLstring.h>
#include <COL/COLmath.h>
#include "FMTlineFeedConvert.h"

#include <COL/COLostream.h>
#include <string.h>
#include <stdio.h>

#define FMT_NO_TRUNCATE ((COLuint32)-1)

FMTlineFeedConvert::FMTlineFeedConvert(const char* pString, const char* LineFeed)
:  m_pString(pString), 
   m_pLineFeed(LineFeed)
{
}

FMTlineFeedConvert::FMTlineFeedConvert(const FMTlineFeedConvert& Original): m_pString(Original.m_pString), m_pLineFeed(Original.m_pLineFeed)
{
}

FMTlineFeedConvert& FMTlineFeedConvert::operator=(const FMTlineFeedConvert& Original)
{
   m_pString = Original.m_pString;
   m_pLineFeed = Original.m_pLineFeed;
   return *this;
}

void FMTlineFeedConvert::PrintOn(COLostream& Stream) const
{
   COLstring Input = m_pString;
   COLstring LineFeed = m_pLineFeed;
   Stream << FMTmakeMessagePrintable(Input,LineFeed,FMT_NO_TRUNCATE) ;
}

COLstring FMTmakeMessagePrintable(const COLstring& InputString, const COLstring& LineFeed, COLuint32 MaximumLength) 
{

   //we'll truncate if maximum length is not -1
   COLstring Input = InputString;
   if(MaximumLength != FMT_NO_TRUNCATE && MaximumLength < InputString.size())
   {
      COLint32 OriginalLength = InputString.size();
      Input = Input.substr(0,MaximumLength);
      COLostream Stream(Input);
      Stream <<"<message output is truncated to " << MaximumLength
             <<" bytes from original length of " << OriginalLength << " bytes>" <<newline;
   }
   

   COLmath::resetRandomGenerator();
   COLstring Placeholder; //min = 128 max = 255
   Placeholder.append(1, (char)(COLmath::randomNumber(127) + 128));
   COLuint32 Sentinel = 0;

   //to prevent overlaps, the Placeholder must be a completely unique sequence in the string
   //and the replacement
   while( Input.find(Placeholder) != npos ||
          Placeholder.find(Input) != npos ||
          LineFeed.find(Placeholder) != npos ||
          Placeholder.find(LineFeed) != npos )
   {
      Placeholder += (char)COLmath::randomNumber(127) + 128;
      Sentinel++;
      //this prevents infinite looping
      COLPOSTCONDITION(Sentinel < 1000);
   }
        
   Input.replace(LineFeed,Placeholder);

   //replace the \rs and \ns
   Input.replace("\r",Placeholder);
   Input.replace("\n",Placeholder);

   //replace the placeholder
   Input.replace(Placeholder,LineFeed);

   return Input;
}

COLostream& operator<<(COLostream& Stream, const FMTlineFeedConvert& Formatter)
{
   Formatter.PrintOn(Stream);
   return Stream;
}
