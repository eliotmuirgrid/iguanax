// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOChex
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 19 August 2025 - 11:52AM
// ---------------------------------------------------------------------------

#include <DOC/DOChex.h>

#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>

COL_LOG_MODULE;

COLstring DOCtoHex(COLuint64 Number){
   COLostreamString HexOut;
   HexOut << hex << Number;
   return COLstring(HexOut.str()).toLowerCase();
}

int DOChexCharToInt(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
    if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
    return -1; // invalid character
}

COLuint64 DOChexStringToInt(const char* hexStr) {
   COL_FUNCTION(DOChexCharToInt);
   COLuint64 result = 0;
   while (*hexStr) {
      int digit = DOChexCharToInt(*hexStr++);
      if (digit < 0) {
         // invalid character found
         break;
      }
      result = (result << 4) | digit; // multiply by 16 and add digit
   }
   return result;
}
COLuint64 DOChexStringToInt(const COLstring& hexStr){
   COL_FUNCTION(DOChexStringToInt);
   return DOChexStringToInt(hexStr.c_str());
}



COLstring DOCpaddHex(const COLstring& Hex){
   COL_FUNCTION(DOCpaddHex);
   if (Hex.size() < 16){
      return COLstring(16-Hex.size(), '0') + Hex;
   }
   return Hex;
}

void DOCwebHexStringToInt(const COLwebRequest& R){
   COL_FUNCTION(DOCwebHexStringToInt);
   const COLvar& Data = R.Data;
   COLstring created = COLvarRequiredString(Data, "created");
   
   const COLuint64 unix = DOChexStringToInt(created);

   COLvar Result;

   if (!unix){
      return COLrespondError(R.Address, "Conversion to unix time failed.");
   }
   Result["date"] = COLuint64ToString(unix);
   COL_VAR(Result.json(1));
   COLrespondSuccess(R.Address, Result);
   
}
