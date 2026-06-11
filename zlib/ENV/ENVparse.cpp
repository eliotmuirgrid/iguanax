//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: ENVparse.cpp
//
//  Description:
//
//  Implementation
//
//  Author: Matthew Sobkowski
//  Date:   3/24/23 11:08 AM
//  ---------------------------------------------------------------------------
#include <ENV/ENVparse.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void ENVparse(COLstring* pEnv, ENVmap* pEnvMap, COLstring* pErr) {
   COL_FUNCTION(ENVparse);
   pEnv->replace("\r\n", "\n");
   // TODO what sort of errors are necessary?
   COLstring Key, Value;
   bool InKey = true;
   char* pBuffer = pEnv->get_buffer();
   while(*pBuffer) {
      char c = *pBuffer;
      COL_DBG(c);
      if(c == '=') {
         if(InKey) { // TODO worth accounting for having '=' in a value? might be useful for URLs"
            InKey = false;
            Value.clear();
         } else { Value += c; }

      } else if(c == '\n') {
         if(!Key.is_null() && !Value.is_null()) {
            pEnvMap->Map.add(Key, Value);
            Key.clear();
            Value.clear();
            COL_DBG(Key << "=" << Value);
         }
         InKey = true;
      } else if(c != '\r' && c != '\t') { // TODO should we skip these or allow any and all
         InKey ? Key += c : Value += c;
      }
      pBuffer++;
   }
   // Check if we have a final key-value pair that hasn't been added yet
   if(!Key.is_null() && !Value.is_null()) { pEnvMap->Map.add(Key, Value); }
}