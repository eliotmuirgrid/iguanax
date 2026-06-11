 //------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module:  FMTbinaryDump Implementation
//
// Description
//
// Simple class which takes the buffer pointed to and outputs a
// binary dump of the message received.  The implementation is not
// terribly efficient - it's just intended to give a good quick and
// easy diagnostic printout.
//
// Author: Eliot Muir
// Date:   Thu 04/27/2000 
//
//---------------------------------------------------------------------------
#include "FMTprecomp.h"
#pragma hdrstop

#include <stdlib.h>
#include <COL/COLerror.h>
#include "FMTbinaryDump.h"

static const char HexDigits[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

void FMTtoHex(COLsink& Sink, COLuint8 Character)
{
   char Buffer[2];
   Buffer[0] = HexDigits[Character >> 4];
   Buffer[1] = HexDigits[Character & 0xf];
   Sink.write(Buffer, 2);
}

void FMTtoHex(COLostream& Stream, COLuint8 Character)
{
   char Buffer[2];
   Buffer[0] = HexDigits[Character >> 4];
   Buffer[1] = HexDigits[Character & 0xf];
   Stream.write(Buffer, 2);
}

// TODO pretty inefficient implementation - we could use OS functions like
// isnum etc.  We probably should wrap those calls in COLmath - like to profile
// performance against using look up tables.  This could be worthwhile since TREe
// depends on these routintes for serializing TREvariantBinary objects as strings.
COLuint8 FMTfromHex(char Character)
{
   if (Character >= '0' && Character <= '9')
   { //0 - 9
      return Character-'0';
   }
   else if (Character >= 'A' && Character <= 'F')
   { //A - F
      return Character-'A'+10;
   }
   else if (Character >= 'a' && Character <= 'f')
   { //a - f
      return Character-'a'+10;
   }
   else
   {
      COL_ERROR_STREAM("Invalid Hex Character: " << Character,COLerror::PreCondition);
   }
   return 0;
}

bool FMTfromHexIsValid(char Character)
{
   if (Character >= '0' && Character <= '9')
   { //0 - 9
      return true;
   }
   else if (Character >= 'A' && Character <= 'F')
   { //A - F
      return true;
   }
   else if (Character >= 'a' && Character <= 'f')
   { //a - f
      return true;
   }
   return false;
}

bool FMTfromHexIsValid(char UpperHex, char LowerHex)
{
   return FMTfromHexIsValid(UpperHex) && FMTfromHexIsValid(LowerHex);
}

// Hmmm - inefficient - we could make more efficient - HEX library?  See
// comments about TREe dependency.  Also it could be handy to have HEX library
// optimized?
COLuint8 FMTfromHex(char UpperHex, char LowerHex)
{
   COLuint8 Upper = FMTfromHex(UpperHex);
   COLuint8 Lower = FMTfromHex(LowerHex);
   Upper = Upper << 4;
   Upper |= Lower;
   return Upper;
}

void FMTbinaryDump16
(
   COLsink& Sink, 
   const COLuint8* pBuffer,
   COLuint32 Length
)
{
   COLPRECONDITION(Length <= 16)
   COLuint32 CharIndex;
   COLuint32 OutBufIndex = 0;
   char OutBuffer[80];
   for (CharIndex = 0; CharIndex < Length; CharIndex++)
   {
      unsigned char Ch = pBuffer[CharIndex];
      OutBuffer[OutBufIndex++] = HexDigits[Ch >> 4];
      OutBuffer[OutBufIndex++] = HexDigits[Ch & 0xf];
      OutBuffer[OutBufIndex++] = ' ';
      if (CharIndex == 7 || CharIndex == 15)
      {
         OutBuffer[OutBufIndex++] = ' ';
         OutBuffer[OutBufIndex++] = ' ';
      }
   }
   for (; CharIndex < 16; CharIndex++)
   {
      OutBuffer[OutBufIndex++] = ' ';
      OutBuffer[OutBufIndex++] = ' ';
      OutBuffer[OutBufIndex++] = ' ';
      if (CharIndex == 7 || CharIndex == 15)
      {
         OutBuffer[OutBufIndex++] = ' ';
         OutBuffer[OutBufIndex++] = ' ';
      }
   }

   for (CharIndex = 0; CharIndex < Length; CharIndex++)
   {
      if (pBuffer[CharIndex] < 0x20 || pBuffer[CharIndex] > 0x7f)
      {
         OutBuffer[OutBufIndex++] = '.';
      }
      else
      {
         OutBuffer[OutBufIndex++] = pBuffer[CharIndex];
      }
      if (CharIndex == 7)
      {
         OutBuffer[OutBufIndex++] = ' ';
      }
   }
   if (CharIndex < 8)
   {
      OutBuffer[OutBufIndex++] = ' ';
   }
   COLASSERT(OutBufIndex < sizeof(OutBuffer));
   Sink.write(OutBuffer, OutBufIndex);
}

void FMTbinaryDump
(
   COLsink& Sink, 
   const COLuint8* pBuffer, 
   size_t Length,
   const char* newline 
)
{
   COLuint32 NewLineLength = strlen(newline);
   size_t RemainingLength = Length;
   size_t CharIndex;
   for (CharIndex = 0; CharIndex < Length; CharIndex += 16)
   {
      COLuint32 Max = 16;
      if (Max > RemainingLength)
      {
         Max = RemainingLength;
      }
      RemainingLength-=16;

      FMTbinaryDump16(Sink,pBuffer+CharIndex,Max);
      Sink.write(newline,NewLineLength);  
   }
}

//Old operators for backward compatibility
void FMTbinaryDump
(
   COLostream& Stream,
   const COLuint8* pBuffer, 
   size_t Length  
)
{   
   FMTbinaryDump(*Stream.sink(),pBuffer,Length,Stream.getNewLine());
}

void FMTstreamHexImp(COLostream& OutStream, const void* pBuffer, COLindex Length, const char* Newline)
{
   FMTbinaryDump(*OutStream.sink(), (COLuint8*)pBuffer, Length, Newline);
}

COLostreamManip* FMTstreamHex(const void* pBuffer, COLindex Length, const char* Newline)
{
   return new COLostreamManip3<const void*, COLindex, const char*>(FMTstreamHexImp, pBuffer, Length, Newline);
}

