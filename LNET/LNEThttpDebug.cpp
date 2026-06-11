// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LNEThttpDebug
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Thursday 29 June 2023 - 10:49AM
// ---------------------------------------------------------------------------

#include <LNET/LNEThttpDebug.h>
#include <LNET/LNEThttpCurl.h>
#include <LNET/LNETcurl.h>
#include <CURL/CURLutils.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void LNEThttpDebugStartTracing(LNEThttpCurlItem* pItem){
   COL_FUNCTION(LNEThttpDebugStartTracing);
   if (pItem->m_pDebugLog) {
      delete pItem->m_pDebugLog;
      pItem->m_pDebugLog = NULL;
   }
   pItem->m_pDebugLog = new CURLdebugLog();
   pItem->m_pDebugLog->trace(pItem->m_pCurl->m_pHandle);
}

void LNEThttpDebugAppendError(LNEThttpCurlItem* pItem, const COLerror& Error) {
   COL_FUNCTION(LNEThttpDebugAppendError);
   if (pItem->m_pDebugLog) {
      LNETcurlAppendDebugLog(*(pItem->m_pDebugLog), Error);
   } else {
      throw Error;
   }
}

bool LNEThttpDebugIsTracing(const LNEThttpCurlItem* pItem) {
   COL_FUNCTION(LNEThttpDebugIsTracing);
   if (!pItem->m_pDebugLog) {
      return false;
   }
   return pItem->m_pDebugLog->tracing();
}

const COLstring& LNEThttpDebugReadOutput(const LNEThttpCurlItem* pItem) {
   COL_FUNCTION(LNEThttpDebugReadOutput);
   if (!pItem->m_pDebugLog) {
      COL_TRC("Debugging was never enabled in the first place...");
      // Easiest way to grab a const reference to empty string
      return COLvar().str();
   }
   return pItem->m_pDebugLog->Output;
}
