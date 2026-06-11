// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDlog
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thursday 18 May 2023 - 02:56PM
// ---------------------------------------------------------------------------
#include <BLUA/BLUAmanager.h>
#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
#include <DBD/DBDlog.h>
#include <LOG/LOGlog.h>
#include <SCK/SCKloop.h>
#include <SDB/SDBcapi.h>
COL_LOG_MODULE;

static void DBDlogMessage(COLstring ComponentId, COLstring Data, SDBmessageId Related, COLstring HashTags,
                          BLUAinstanceManager* pManager) {
   COLstring Error;
   SDBid Id = SDBIlogComponent(Data, ComponentId, HashTags, Related, COLvar(), &Error);
   if (Id.isNull() && !Error.is_null()) {
      // Data too big. log an error instead
      SDBIlogComponent(Error, ComponentId, "#error", Related, COLvar(), &Error);
      // TODO: return here if we don't want to increment error count for iguana.log
   }
   auto Instance = pManager->m_Instances.find(ComponentId);
   if (SDBhasTag(HashTags, "#error") && pManager->m_Instances.end() != Instance) {
      // increment error count (iguana 6 behaviour)
      Instance.value()->m_Config.Status.ErrorCount++;
   }
}

void DBDlogMessage(const LOGlogInfo& LogInfo, const COLstring& Data, SCKloop* pLoop, BLUAinstanceManager* pManager) {
   SDBmessageId Related(LogInfo.m_Source, LogInfo.m_DateTime, LogInfo.m_Index);
   pLoop->post(COLnewClosure0(&DBDlogMessage, LogInfo.m_ComponentId, Data, Related, LogInfo.m_HashTags,
                              pManager));
}