// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBrequestParseHeaders
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Thursday 26 October 2023 - 04:34PM
// ---------------------------------------------------------------------------

#include <WEB/WEBrequestParseHeaders.h>
#include <WEB/WEBrequest.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

bool WEBrequestParseHeaders(const char* pStart, const char* pEnd, COLmap<COLstring, COLstring>* pHeaders){
   COL_FUNCTION(WEBrequestParseHeaders);
   const char* pCursor = pStart;
   COLmap<COLstring, COLstring>& Headers = *pHeaders;
   do{
      const char* pColon = WEBsafeStrStr(pCursor, ":", pEnd-pCursor);
      if (pColon == NULL) {
         COL_TRC("Failed to find colon");
         return false;
      }
      COL_DUMP("Header", pCursor, pColon-pCursor);
      COLstring Key(pCursor, pColon-pCursor);
      Key = Key.toLowerCase();
      COL_DUMP("Data:" , pColon, pEnd-pColon);
      const char* pEOL = WEBsafeStrStr(pColon, "\r\n", pEnd-pColon);
      if (pEOL == NULL) {
         COL_TRC("Failed to find pEOL");
         return false;
      }
      COLstring Value(pColon+2, pEOL-pColon-2);
      pCursor = pEOL + 2;
      COL_VAR2(Key, Value);
      Headers.add(Key, Value);
   } while (pCursor != 0 && pCursor < pEnd);
   return true;
}