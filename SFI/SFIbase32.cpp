//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIbase32
//
// Description:
//
// Implementation
//
// Author: Steven Dodd
// Date:   Thursday, August 19th, 2010 @ 02:55:40 PM
//
//---------------------------------------------------------------------------
#include "SFIprecomp.h"
#pragma hdrstop

#include "SFIbase32.h"

#include <COL/COLsink.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

const char* SFIbase32::BASE32_CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

void SFIbase32::encode(COLsink& Destination, const COLuint8* pSource, COLuint32 SourceSize)
{
   char Buffer[8];
   
   for (; SourceSize > 5; pSource += 5, SourceSize -= 5)
   {
      encodeTuple(Buffer, pSource);
      Destination.write(Buffer, 8);
   }
   
   if (SourceSize)
   {
      COLuint8 Tuple[5] = { 0 };
      memcpy(Tuple, pSource, SourceSize);
      encodeTuple(Buffer, Tuple);

      switch (SourceSize)
      {
      case 1: memset(&Buffer[2], '=', 6); break;
      case 2: memset(&Buffer[4], '=', 4); break;
      case 3: memset(&Buffer[5], '=', 3); break;      
      case 4: memset(&Buffer[7], '=', 1); break;      
      }

      Destination.write(Buffer, 8);
   }
}

void SFIbase32::encodeTuple(char Destination[8], const COLuint8 Source[5])
{
   COLuint64 SourceValue = 0;
   for (COLuint32 SourceIndex = 0; SourceIndex < 5; ++SourceIndex)
   {
      SourceValue *= 256;
      SourceValue |= Source[SourceIndex];
   }

   for (COLuint32 DestinationIndex = 0; DestinationIndex < 8; ++DestinationIndex)
   {
      Destination[7-DestinationIndex] = BASE32_CHARS[SourceValue % 32];
      SourceValue /= 32;
   }
}
