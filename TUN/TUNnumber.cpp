// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNnumber
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 14 June 2023 - 08:33AM
// ---------------------------------------------------------------------------

#include <TUN/TUNnumber.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// similar to Google's Varint encoding used in protocls buffers.
int TUNencodeNumber(COLuint32 Value, char* pStart){
   COL_FUNCTION(TUNencodeNumber);
   char* pOut = pStart;
   do {
      *pOut = (Value & 0x7F) | ((Value > 0x7F) << 7);
      Value >>= 7;
      pOut++;
   } while (Value != 0);
   return pOut-pStart;
}

int TUNdecodeNumber(COLuint32* pValue, const unsigned char* pStart){
   COL_FUNCTION(TUNdecodeNumber);
   COLuint32& value = *pValue;
   value = 0;
   int i = 0;
   while (1) {
      COLuint32 byteValue = pStart[i]; // Cast to unsigned, so that the shift doesn't propagate the sign bit
      COLuint32 sevenBits = byteValue & 0x7F;
      value |= sevenBits << (7 * i);
      i++;
      // If the most significant bit is not set, this is the last byte
      if ((byteValue & 0x80) == 0) {
         break;
      }
      if (i > 4){
         COL_ERR("Overflow");
         value = 0;
         return -1;
      }
   }
   COL_VAR2(value, i);
   return i;
}


void TUNnumberWrite(COLstring* pOut, COLuint32 Value){
   COL_FUNCTION(TUNnumberWrite);
   int SizeRequired = pOut->size() +5;
   if (pOut->capacity() <= SizeRequired){
      pOut->setCapacity(SizeRequired);
   }
   int SizeOfNumber = TUNencodeNumber(Value, pOut->get_buffer()+ pOut->size());
   COL_VAR(SizeOfNumber);
   pOut->setSize(SizeOfNumber+ pOut->size());
}
   