//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: REGguid
//
// Description:
//
// Routines for wrapping GUID access routines
//
// Author: Eliot Muir
// Date:   Mon 28/06/1999 
//
//---------------------------------------------------------------------------
#include "REGprecomp.h"
#pragma hdrstop

#include "REGguid.h"

#include <COL/COLmath.h>

REGguidResult REGcreateGuid(REGguid& Guid)
{
   COLmath::resetRandomGenerator();
   Guid.Data[0] = COLmath::randomNumber(0xFF);
   Guid.Data[1] = COLmath::randomNumber(0xFF);
   Guid.Data[2] = COLmath::randomNumber(0xFF);
   Guid.Data[3] = COLmath::randomNumber(0xFF);
   Guid.Data[4] = COLmath::randomNumber(0xFF);
   Guid.Data[5] = COLmath::randomNumber(0xFF);
   Guid.Data[6] = COLmath::randomNumber(0xFF);
   Guid.Data[7] = COLmath::randomNumber(0xFF);
   Guid.Data1 = (COLmath::randomNumber(0xFF) << 24) | (COLmath::randomNumber(0xFF) << 16) 
                | (COLmath::randomNumber(0xFF) << 8) | COLmath::randomNumber(0xFF);
   Guid.Data2 = (COLmath::randomNumber(0xFF) << 8) | COLmath::randomNumber(0xFF);
   Guid.Data3 = (COLmath::randomNumber(0xFF) << 8) | COLmath::randomNumber(0xFF);
   return REG_GUID_SUCCESS;
}

void REGpaddHex(COLostream& Stream, COLuint8 Byte)
{
   if (Byte < 0x10)
   {
      Stream << '0';
   }
   Stream << (COLuint16)Byte;
}

COLstring REGguidToString(const REGguid& Guid)
{
   COLstring Result;
   COLostream Stream(Result);
   Stream << hex;
   REGpaddHex(Stream, ((COLuint8*)&Guid.Data1)[3]);
   REGpaddHex(Stream, ((COLuint8*)&Guid.Data1)[2]);
   REGpaddHex(Stream, ((COLuint8*)&Guid.Data1)[1]);
   REGpaddHex(Stream, ((COLuint8*)&Guid.Data1)[0]);
   Stream << '-';
   REGpaddHex(Stream, ((COLuint8*)&Guid.Data2)[1]);
   REGpaddHex(Stream, ((COLuint8*)&Guid.Data2)[0]);
   Stream << '-'; 
   REGpaddHex(Stream, ((COLuint8*)&Guid.Data3)[1]);
   REGpaddHex(Stream, ((COLuint8*)&Guid.Data3)[0]);
   Stream << '-';
   REGpaddHex(Stream, Guid.Data[0]);
   REGpaddHex(Stream, Guid.Data[1]);
   Stream << '-';
   REGpaddHex(Stream, Guid.Data[2]);
   REGpaddHex(Stream, Guid.Data[3]);
   REGpaddHex(Stream, Guid.Data[4]);
   REGpaddHex(Stream, Guid.Data[5]);
   REGpaddHex(Stream, Guid.Data[6]);
   REGpaddHex(Stream, Guid.Data[7]);
   return Result;
}

COLuint8 REGuint4FromHex(char Character)
{
   switch (Character)
   {
   case '0': return 0;
   case '1': return 1;
   case '2': return 2;
   case '3': return 3;
   case '4': return 4;
   case '5': return 5;
   case '6': return 6;
   case '7': return 7;
   case '8': return 8;
   case '9': return 9;
   case 'a': return 0xA;
   case 'b': return 0xB;
   case 'c': return 0xC;
   case 'd': return 0xD;
   case 'e': return 0xE;
   case 'f': return 0xF;
   case 'A': return 0xA;
   case 'B': return 0xB;
   case 'C': return 0xC;
   case 'D': return 0xD;
   case 'E': return 0xE;
   case 'F': return 0xF;
   default:
      return 255;
   };
}

COLuint8 REGuint8FromHex(const char* pHexNumber)
{
   COLuint8 FirstDigit = 0;
   COLuint8 SecondDigit = 1;
   FirstDigit = REGuint4FromHex(pHexNumber[0]);
   if (FirstDigit == 255)
   {
      COL_ERROR_STREAM_PLAIN(pHexNumber[0] << " found.", REG_ILLEGAL_CHARACTER);
   }
   SecondDigit = REGuint4FromHex(pHexNumber[1]);
   if (SecondDigit == 255)
   {
      COL_ERROR_STREAM_PLAIN(pHexNumber[1] << " found.", REG_ILLEGAL_CHARACTER);
   }

   COLuint8 Result = (FirstDigit << 4) | SecondDigit;
   return Result;
}

bool REGstringToGuid(const COLstring& String, REGguid& Guid)
{
   try
   {
      if (String.size() < 36)
      {
         return false;
      }
      const char* pString = String.c_str();

      ((COLuint8*)&Guid.Data1)[3] = REGuint8FromHex(pString);
      ((COLuint8*)&Guid.Data1)[2] = REGuint8FromHex(pString+2);
      ((COLuint8*)&Guid.Data1)[1] = REGuint8FromHex(pString+4);
      ((COLuint8*)&Guid.Data1)[0] = REGuint8FromHex(pString+6);
      // skip -
      ((COLuint8*)&Guid.Data2)[1] = REGuint8FromHex(pString+9);
      ((COLuint8*)&Guid.Data2)[0] = REGuint8FromHex(pString+11);
      // skip -
      ((COLuint8*)&Guid.Data3)[1] = REGuint8FromHex(pString+14);
      ((COLuint8*)&Guid.Data3)[0] = REGuint8FromHex(pString+16);
      // skip -
      Guid.Data[0] = REGuint8FromHex(pString+19);
      Guid.Data[1] = REGuint8FromHex(pString+21);
      // skip -
      Guid.Data[2] = REGuint8FromHex(pString+24);
      Guid.Data[3] = REGuint8FromHex(pString+26);
      Guid.Data[4] = REGuint8FromHex(pString+28);
      Guid.Data[5] = REGuint8FromHex(pString+30);
      Guid.Data[6] = REGuint8FromHex(pString+32);
      Guid.Data[7] = REGuint8FromHex(pString+34);

      return true;    
   }
   catch(const COLerror& Error)
   {
      return false;
   }
}

bool REGguidMatch(const REGguid& First, const REGguid& Second)
{
   if (First.Data1 != Second.Data1)
   {
      return false;
   }
   if (First.Data2 != Second.Data2)
   {
      return false;
   }
   if (First.Data3 != Second.Data3)
   {
      return false;
   }
   for (size_t DataIndex = 0;
        DataIndex < 8;
        DataIndex++)
   {
      if (First.Data[DataIndex] != Second.Data[DataIndex])
      {
         return false;
      }
   }
   return true;
}
