// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBnumber
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thursday 16 February 2023 - 04:47PM
// ---------------------------------------------------------------------------
#include <cmath>
#include <SDB/SDBnumber.h>

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

int SDBnumberSize(COLuint64 Value) {
   // COL_FUNCTION(SDBwriteNumber);
   if (Value == 0) { return 1; }
   int MSB = 0;
   while (Value > 0) {
      Value >>= 1;
      MSB++;
   }
   return ceil(MSB/7.0);
}

int SDBwriteNumber(COLuint64 Value, char* pBuffer) {
   // COL_FUNCTION(SDBwriteNumber);
   char* pOut = pBuffer;
   do {
      *pOut = (Value & 0x7F) | ((Value > 0x7F) << 7);
      Value >>= 7;
      pOut++;
   } while (Value != 0);
   return pOut-pBuffer;
}

int SDBreadNumber(COLuint64* pValue, const char* pBuffer, int BufSize) {
   // COL_FUNCTION(SDBreadNumber);
   if (BufSize <= 0) { return -1; } // need more data
   static int MaxSize = SDBnumberSize(COL_UINT_64_MAX);
   COLuint64& Value = *pValue;
   Value = 0;
   int i = 0;
   while (true) {
      COL_VAR(i);
      COLuint64 Byte = pBuffer[i]; // Cast to unsigned, so that the shift doesn't propagate the sign bit
      COLuint64 Data = Byte & 0x7F;
      Value |= Data << (7 * i);
      i++;
      // If the most significant bit is not set, this is the last byte
      if ((Byte & 0x80) == 0) { break; }
      // expecting another byte
      if (i == MaxSize) {  // parsed MaxSize bytes yet still wanting more
         COL_ERR("Not a number. Must be " << MaxSize << " bytes or less");
         Value = 0;
         return -3;
      }
      if (BufSize > i && pBuffer[i] == 0) {  // next byte is 0
         COL_ERR("Not a number. Expecting another byte");
         Value = 0;
         return -2;
      }
      if (BufSize == i) {  // reached the end of buffer
         COL_ERR("Need more data");
         Value = 0;
         return -1;
      }
   }
   COL_VAR2(Value, i);
   return i;
}
