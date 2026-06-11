//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIbase64DecodeFilter
//
// Description:
//
// Used for base 64 conversion of a string.
//
// based on: http://jabberd.jabberstudio.org/1.4/codedoc/base64_8c.html
//
// Author: Rob Moyse
// Date: July 15, 2004 
//
//---------------------------------------------------------------------------
#include "SFIprecomp.h"
#pragma hdrstop

#include "SFIbase64.h"
#include "SFIbase64DecodeFilter.h"

#include <COL/COLsinkString.h>

#include <string.h>

// g_LookupCharValue and g_LookupValid definitions were generated.
// See GENERATE_CODE_FOR_STATIC_SFI_ARRAYS.

static char g_LookupCharValue[256] =
{
   127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
   127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
   127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 62, 127, 127, 127, 63,
   52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 127, 127, 127, 127, 127, 127,
   127, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
   15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 127, 127, 127, 127, 127,
   127, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
   41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 127, 127, 127, 127, 127,
   127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
   127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
   127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
   127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
   127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
   127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
   127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
   127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127
};

static char g_LookupValid[256] =
{
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0,
   0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
   0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

COLuint32 SFIbase64DecodeFilter::write(const void* cpBuffer, COLuint32 SizeOfBuffer ) {
   //group into 4
   const unsigned char* pData = (const unsigned char*)cpBuffer;
   COLuint32 CurrentDataIndex = 0;
   while (CurrentDataIndex < SizeOfBuffer) {
      while (CurrentDataIndex < SizeOfBuffer && CurrentGroupIndex < 4) {
         if ( g_LookupValid[ pData[CurrentDataIndex] ]) {
            //this ignores invalid characters
            CurrentGroup[CurrentGroupIndex] = g_LookupCharValue[pData[CurrentDataIndex] ];
            CurrentGroupIndex++;
         }
         CurrentDataIndex++;
      }
      if (CurrentGroupIndex == 4) {  
         COLint32 BytesToDecode = 3;
         bool OnlyEqualsYet = true;

         // check if the characters are valid */
         for (COLint32 CharacterIndex = 3; (BytesToDecode > 0) && (CharacterIndex >=0); CharacterIndex--) {
            if (CurrentGroup[CharacterIndex] == 127) { //means that its the '=' character
               if(!OnlyEqualsYet) {
                  COL_ERROR_STREAM_PLAIN("Invalid '=' in decoding Base64 stream", COLerror::PreCondition);
               }               
               // we will ignore this character anyway, make it something
               // that does not break our calculations
               CurrentGroup[CharacterIndex] = 0;
               BytesToDecode--;
               continue;
            }

            // after we got a real character, no other '=' are allowed anymore
            OnlyEqualsYet = false;
         }

         COLPRECONDITION(BytesToDecode >= 0)
         if (BytesToDecode) {
            //decode
            COLint32 TripleValue;

            TripleValue = CurrentGroup[0];
            TripleValue *= 64;
            TripleValue += CurrentGroup[1];
            TripleValue *= 64;
            TripleValue += CurrentGroup[2];
            TripleValue *= 64;
            TripleValue += CurrentGroup[3];

            // break the big value into bytes
            for (COLuint32 ByteIndex = BytesToDecode; ByteIndex<3; ByteIndex++) {
               TripleValue /= 256;
            }

            char TripleResult[3];
            for (COLint32 ResultIndex = BytesToDecode-1; ResultIndex>=0; ResultIndex--) {
               TripleResult[ResultIndex] = TripleValue%256;
               TripleValue /= 256;
            }

            next().write(TripleResult,BytesToDecode);
         }
         
         CurrentGroupIndex = 0;
      }
   }
   return SizeOfBuffer;
}

void SFIbase64DecodeFilter::resetFilter() {
   CurrentGroupIndex = 0;
}

void SFIbase64DecodeFilter::onEndStream() {
   next().onEndStream();
}

#ifdef GENERATE_CODE_FOR_STATIC_SFI_ARRAYS
class SFIbase64DecodeFilterLookup
{
public:
   SFIbase64DecodeFilterLookup()
   {
      //init to invalid
      COLuint32 LookupIndex = 0;
      for (LookupIndex = 0; LookupIndex < 256; LookupIndex++)
      {
         LookupValid[LookupIndex] = false;
      }
      //set the valid chars
      COLuint32 Base64CharLength = strlen(SFIbase64::BASE64_CHARS);
      COLuint32 Base64Index = 0;
      for (Base64Index = 0; Base64Index < Base64CharLength; Base64Index++)
      {
         LookupValid[SFIbase64::BASE64_CHARS[Base64Index]] = true;
      }
      LookupValid[ '=' ] = true; //the padding character

      //char values
      for (LookupIndex = 0; LookupIndex < 256; LookupIndex++)
      {
         LookupCharValue[LookupIndex] = 127; //invalid char value
      }
      for (Base64Index = 0; Base64Index < Base64CharLength; Base64Index++)
      {
         LookupCharValue[SFIbase64::BASE64_CHARS[Base64Index] ] = Base64Index;
      }
      dump("static char g_LookupCharValue", LookupCharValue);
      dump("static char g_LookupValid", LookupValid);
   }
   static void dump(const char* arrayName, const char* array)
   {
      COLcout << arrayName << "[256] =\n{";
      for (int i = 0; i < 256; ++i)
      {
         COLcout << ((i%16 == 0) ? "\n   " : ", ");
         COLcout << (int)(signed char)array[i];
         if (i%16 == 15 && i != 255) COLcout << ',';
      }
      COLcout << "\n};\n";
   }
   static SFIbase64DecodeFilterLookup& instance()
   {
      static SFIbase64DecodeFilterLookup Instance;
      return Instance;
   }         
   char LookupCharValue[256];
   char LookupValid[256];
};
static SFIbase64DecodeFilterLookup g_Instance;
#endif



void SFIbase64Decode(const COLstring& Input, COLstring* pOutput){
   COLsinkString Sink(*pOutput);
   SFIbase64DecodeFilter Filter(&Sink);
   Filter.write(Input.c_str(), Input.size());
   Filter.onEndStream();
}

