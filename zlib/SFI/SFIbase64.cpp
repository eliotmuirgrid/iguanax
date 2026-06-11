//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIbase64
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
//
//---------------------------------------------------------------------------
#include "SFIprecomp.h"
#pragma hdrstop

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

\
#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include "SFIbase64.h"

const char* SFIbase64::BASE64_CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
 
COLstring SFIbase64::encode(const COLuint8* pSourceBuffer, COLuint32 SourceLength) {
   COL_FUNCTION(SFIbase64::encode);
   COLstring ResultBuffer;
   ResultBuffer.setCapacity( ( (SourceLength+2)/3 ) * 4 + 2);
   ResultBuffer.setSize( ( (SourceLength+2)/3 ) * 4 + 1);
   COLuint8* pResultBuffer = (COLuint8*)ResultBuffer.c_str();

   // encode all full triples
   while (SourceLength >= 3) {
      encodeTriple(pSourceBuffer, pResultBuffer);
      SourceLength -= 3;
      pSourceBuffer += 3;
      pResultBuffer += 4;
   }

   // encode the last one or two characters
   if (SourceLength > 0) {
      COLuint8 RemainingCharacters[3];
      memset(RemainingCharacters, 0, sizeof(RemainingCharacters));
      memcpy(RemainingCharacters, pSourceBuffer, SourceLength);
      encodeTriple(RemainingCharacters, pResultBuffer);

      pResultBuffer[3] = '=';
      if (SourceLength == 1) {
         pResultBuffer[2] = '=';
      }

      pResultBuffer += 4;
   }

   // terminate the string
   pResultBuffer[0] = 0;
   return ResultBuffer;
}

void SFIbase64::encodeTriple(const COLuint8 Triple[3], COLuint8 Result[4]) {
   COL_FUNCTION(SFIbase64::encodeTriple);
   COLuint32 TripleValue = Triple[0];
   TripleValue *= 256;
   TripleValue += Triple[1];
   TripleValue *= 256;
   TripleValue += Triple[2];

   for (COLuint32 ResultIndex = 0; ResultIndex < 4; ResultIndex++) {
      Result[3-ResultIndex] = BASE64_CHARS[TripleValue%64];
      TripleValue /= 64;
   }
}

COLstring SFIbase64::decode(const char* pSourceBuffer) {
   COL_FUNCTION(SFIbase64::decode);
   COLstring ResultBuffer;
   ResultBuffer.setCapacity( ((strlen(pSourceBuffer)+5)/4) * 3);
   COLuint8* pResultBuffer = (COLuint8*)ResultBuffer.get_buffer();

   // concatinate '===' to the source to handle unpadded base64 data */   
   COLarray<char> Source;
   Source.resize(strlen(pSourceBuffer)+5);
   strcpy(Source.getArray(), pSourceBuffer);
   strcat(Source.getArray(), "====");

   char* pTemp = Source.getArray();

   char Quadruple[4];
   COLuint8 TripleResult[3];

   COLuint32 TempLength = 3;

   // convert as long as we get a full result
   while (TempLength == 3) {
      // get 4 characters to convert
      for (COLuint32 QuadrupleIndex = 0; QuadrupleIndex<4; QuadrupleIndex++) {
         // skip invalid characters - we won't reach the end */
         while (*pTemp != '=' && characterValue(*pTemp) <0) {
            pTemp++;
         }
         Quadruple[QuadrupleIndex] = *(pTemp++);
      }

      // convert the characters */
      TempLength = decodeTriple(Quadruple, TripleResult);

      // put the partial result in the result buffer */
      memcpy(pResultBuffer, TripleResult, TempLength);
      
      pResultBuffer += TempLength;
   }
   COL_VAR(ResultBuffer);
   //ResultBuffer.setSize(ResultBuffer.size() - (ResultBuffer.end() - pResultBuffer));
   return ResultBuffer;
}

COLuint32 SFIbase64::decodeTriple (char Quadruple[4], COLuint8* TripleResult) {
   COL_FUNCTION(SFIbase64::decodeTriple);
   COLint32 CharacterValue[4];

   for (COLuint32 QuadrupleIndex = 0; QuadrupleIndex<4; QuadrupleIndex++) {
      CharacterValue[QuadrupleIndex] = characterValue(Quadruple[QuadrupleIndex]);
   }

   COLint32 BytesToDecode = 3;
   bool OnlyEqualsYet = true;

   // check if the characters are valid */
   for (COLint32 CharacterIndex = 3; CharacterIndex >=0; CharacterIndex--) {
      if (CharacterValue[CharacterIndex]<0) {
         if (OnlyEqualsYet && Quadruple[CharacterIndex] =='=') {
            // we will ignore this character anyway, make it something
            // that does not break our calculations
            CharacterValue[CharacterIndex] = 0;
            BytesToDecode--;
            continue;
         }
         return 0;
      }
      // after we got a real character, no other '=' are allowed anymore
      OnlyEqualsYet = false;
   }
   // if we got "====" as input, bytes_to_decode is -1
   if (BytesToDecode < 0) {
      BytesToDecode = 0;
   }

   // make one big value out of the partial values 
   COLint32 TripleValue;
   TripleValue = CharacterValue[0];
   TripleValue *= 64;
   TripleValue += CharacterValue[1];
   TripleValue *= 64;
   TripleValue += CharacterValue[2];
   TripleValue *= 64;
   TripleValue += CharacterValue[3];

   // break the big value into bytes
   for (COLuint32 ByteIndex = BytesToDecode; ByteIndex<3; ByteIndex++) {
      TripleValue /= 256;
   }

   for (COLint32 ResultIndex = BytesToDecode-1; ResultIndex>=0; ResultIndex--) {
      TripleResult[ResultIndex] = TripleValue%256;
      TripleValue /= 256;
   }
   return BytesToDecode;
}

COLint32 SFIbase64::characterValue(char Base64Character) {
   COL_FUNCTION(SFIbase64::characterValue);
   if (Base64Character >= 'A' && Base64Character <= 'Z')
      return Base64Character-'A';
   if (Base64Character >= 'a' && Base64Character <= 'z')
      return Base64Character-'a'+26;
   if (Base64Character >= '0' && Base64Character <= '9')
      return Base64Character-'0'+2*26;
   if (Base64Character == '+')
      return 2*26+10;
   if (Base64Character == '/')
      return 2*26+11;
   return -1;
}

void SFIbase64::encodeNonTerminated(const COLuint8* pSourceBuffer, COLuint32 BufferLength, COLstring& OutputBuffer) {
   COL_FUNCTION(SFIbase64::encodeNonTerminated);
   if (!BufferLength) {
      return;
   }
   OutputBuffer = encode(pSourceBuffer,BufferLength);
   COLPRECONDITION(OutputBuffer.size() > 0)
   OutputBuffer.setSize( OutputBuffer.size()-1 );
}

void SFIbase64::decodeNonTerminated(const COLuint8* pSourceBuffer, COLuint32 BufferLength, COLstring& OutputBuffer) {
   COL_FUNCTION(SFIbase64::decodeNonTerminated);
   if (!BufferLength) {
      return;
   }
   if (pSourceBuffer[BufferLength-1] != '\0') {
      //we have to copy out to null terminate
      COLstring Input((const char*)pSourceBuffer, BufferLength);
      OutputBuffer = decode(Input.c_str());
   } else {
      OutputBuffer = decode((const char*)pSourceBuffer);
   }
}