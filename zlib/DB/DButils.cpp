//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DButils
//
// Description:
//
// Implementation
//
// Author: Guillaume Couture-Levesque
// Date:   July 1st, 2008
//
//---------------------------------------------------------------------------
#include "DBprecomp.h"
#pragma hdrstop

#include "DButils.h"
#include "DBdatabase.h"
#include "DBdatabaseMySql.h"
#include "DBdatabaseOciOracle.h"
#include "DBdatabaseOdbc.h"

#include <COL/COLstring.h>
#include <COL/COLsink.h>
#include <ctype.h>

bool DBstringIsAscii(const COLstring& String)
{
   COLindex StringSize = String.size();
   const char* pBuffer = String.c_str();
   for(COLindex CharIndex=0; CharIndex < StringSize; CharIndex++)
   {
      if(!isascii(pBuffer[CharIndex]))
      {
         return false;
      }
   }
   return true;
}

void DBbinaryToHex(COLsink& OutputSink, const void* pVoidBuffer, COLuint32 SizeOfBuffer){
   //we avoid depending on SFI this way
   //else there is an army of projects to update.
   const unsigned char* pBuffer = (const unsigned char*)pVoidBuffer;
   int CharIndex = 0; 
   static const char hex[] = "0123456789abcdef";
   int OutputBufferCount = 0;
   const int OutputBufferSize = 256;    // size *MUST* be an even number.
   char OutputBuffer[OutputBufferSize]; // 256 seems to be optimal for core2 in tests
   for (CharIndex = 0; CharIndex < SizeOfBuffer; CharIndex++)
   {
      unsigned Value = pBuffer[CharIndex];
      OutputBuffer[OutputBufferCount++] = hex[Value >> 4];
      OutputBuffer[OutputBufferCount++] = hex[Value & 0xf];
      if (sizeof(OutputBuffer) == OutputBufferCount)
      {
         OutputSink.write(OutputBuffer, OutputBufferCount);
         OutputBufferCount = 0;
      }
   }
   if (OutputBufferCount > 0)
   {
      OutputSink.write(OutputBuffer, OutputBufferCount);
   }
}
