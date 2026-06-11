//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSHfile
//
// Description:
//
// Implementation
//
// Author: Dmitri Klimenko
// Date:   Friday, July 27th, 2007 @ 12:18:11 PM
//
//---------------------------------------------------------------------------
#include "TSHprecomp.h"
#pragma hdrstop

#include "TSHfile.h"

#include <LEG/LEGvector.h>
#include <FIL/FILfile.h>
#include <FIL/FILutils.h>

bool TSHfileCutExtension(const COLstring& Source, const char* Ext, COLstring& Result)
{
   COLindex ExtSize=strlen(Ext);
   if(Source.size()<=ExtSize)
   {
      return false;
   }
   if(Source.substr(Source.size()-ExtSize, ExtSize) != Ext)
   {
      return false;
   }
   Result = Source.substr(0, Source.size()-ExtSize);
   return true;
}

static void splitLines(const COLstring& Source, LEGvector<COLstring>& Lines)
{
   const char* pSource = Source.c_str();
   unsigned int SourceSize = Source.size();
   if(!SourceSize)
   {
      return;
   }

   COLindex LastStart=0;

   for(unsigned int CharIndex=0; CharIndex < SourceSize ; )
   {
      if(pSource[CharIndex]=='\r' || pSource[CharIndex]=='\n')
      {
         Lines.push_back().append(pSource+LastStart, CharIndex-LastStart);
         if(CharIndex+1 < SourceSize && pSource[CharIndex]=='\r' && pSource[CharIndex+1] == '\n')
         {
            CharIndex += 2;
         }
         else
         {
            CharIndex += 1;
         }
         LastStart = CharIndex;
         continue;
      }
      // none of the above
      CharIndex++;
   }
   // last part - there will always be at least one unless empty
   Lines.push_back().append( pSource + LastStart, SourceSize-LastStart);
};

static COLstring join(const char* Separator, LEGvector<COLstring>& Lines)
{
   if(!Lines.size())
   {
      // COLcout << "no lines" << newline;
      return "";
   };

   COLstring Result = Lines[0];

   //COLcout << "lines[" << Lines.size() << "]:" << newline;
   //COLcout << "(" << Lines[0] << ")" << newline;
   for(COLindex LineIndex=1; LineIndex < Lines.size(); LineIndex++)
   {
      //COLcout << "(" << Lines[LineIndex] << ")" << newline;
      Result += Separator;
      Result += Lines[LineIndex];
   };
   return Result;
}

COLstring TSHfileFixNewLines(const char* NewLine, const COLstring& Source)
{
   LEGvector<COLstring> Lines;
   splitLines(Source, Lines);
   return join(NewLine, Lines);
}

COLstring TSHfileRead( const COLstring& FileName )
{
   COLstring Result;
   FILreadFile(FileName,&Result);
   return Result;
}


void TSHfileWrite( const COLstring& FileName, const COLstring& Data)
{
   FILfile File;;
   File.open(FileName, FILfile::Rewrite);
   File.write(Data.c_str(), Data.size());
}

