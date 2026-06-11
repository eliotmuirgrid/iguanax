//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: UTFconvert
//
// Description:
//
// Implementation
//
// Author: Dmitri Klimenko
// Date:   Tuesday, February 10th, 2009 @ 03:23:42 PM
//
//---------------------------------------------------------------------------
#include "UTFprecomp.h"
#pragma hdrstop

#include "UTFconvert.h"

#include <COL/COLstring.h>

#include "UTFstringWchar.h"
#include "UTFstring16.h"

const char* UTFconversionResultToString(ConversionResult Result)
{
   switch(Result)
   {
   case conversionOK:
      return "conversionOK"; /* conversion successful */
   case sourceExhausted:
      return "sourceExhausted"; /* partial character in source, but hit end */
   case targetExhausted:
      return "targetExhausted"; /* insuff. room in target for conversion */
   case sourceIllegal:
      return "sourceIllegal"; /* source sequence is illegal/malformed */
   }
   return 0; // unexpected
};

void UTFconvertUtf8ToWchar(UTFstringWchar& WcharResult, const COLstring& UtfSource)
{
   const UTF8* SourceStart = (const UTF8*)UtfSource.c_str();
   const UTF8* SourceUsed = SourceStart; // this variable will be updated by the conversion routine to point to the end of consumed source data   
   const UTF8* SourceEnd = SourceStart + UtfSource.size();


   COLindex Utf16BufferSize = UtfSource.size() * 2 + 1; // being stupid and generous here
   UTFstring16 Utf16Buffer;
   Utf16Buffer.setWcharData(0, Utf16BufferSize);
   UTF16* Utf16Start = Utf16Buffer.data();  
   UTF16* Utf16End = Utf16Start + Utf16BufferSize;
   UTF16* Utf16Used = Utf16Start; // this variable will be updated by the conversion routine to point to the end of UTF16 data
   ConversionResult Result = ConvertUTF8toUTF16(&SourceUsed, SourceEnd, &Utf16Used, Utf16End, strictConversion);
   if(Result != conversionOK)
   {
      COL_ERROR_STREAM( "ConvertUTF8toUTF16() returned non-OK result " << UTFconversionResultToString(Result), COLerror::PreCondition );
   }
   
   COLPRECONDITION( Utf16Used >= Utf16Start && Utf16Used < Utf16End);
   COLPRECONDITION( SourceUsed == SourceEnd);
  
   WcharResult.setUtf16Data(Utf16Start, (Utf16Used-Utf16Start));
};

COLstring UTFconvertWcharToUtf8(const UTFstringWchar& Source)
{
   UTFstring16 SourceUtf16;
   SourceUtf16.setWcharData(Source.data(), Source.size());
   
   const UTF16* SourceStart = SourceUtf16.data();
   const UTF16* SourceUsed = SourceStart; // this variable will be updated by the conversion routine to point to the end of consumed source data   
   const UTF16* SourceEnd = SourceStart + Source.size();

   COLindex Utf8BufferSize = Source.size() * 6 + 1; // being VERY stupid and generous here

   COLstring Utf8Buffer;
   Utf8Buffer.assign(Utf8BufferSize, 0);

   UTF8* Utf8Start = (UTF8*)Utf8Buffer.c_str();
   UTF8* Utf8End = Utf8Start + Utf8BufferSize;
   UTF8* Utf8Used = Utf8Start; // this variable will be updated by the conversion routine to point to the end of UTF16 data
   ConversionResult Result = ConvertUTF16toUTF8(&SourceUsed, SourceEnd, &Utf8Used, Utf8End, strictConversion);
   if(Result != conversionOK)
   {
      COL_ERROR_STREAM( "ConvertUTF16toUTF8() returned non-OK result " << UTFconversionResultToString(Result), COLerror::PreCondition );
   }
   
   COLPRECONDITION( Utf8Used >= Utf8Start && Utf8Used < Utf8End);
   COLPRECONDITION( SourceUsed == SourceEnd);

   return COLstring(Utf8Buffer.c_str(), (Utf8Used-Utf8Start));
}
