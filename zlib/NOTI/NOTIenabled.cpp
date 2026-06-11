// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: NOTIenabled.cpp
//
//  Description:
//
//  Implementation
//
//  Author: Matthew Nitsopoulos
//  Date:   9/3/22, 10:53 AM
// ---------------------------------------------------------------------------
#include <NOTI/NOTIenabled.h>
#include <NOTI/NOTImodel.h>
#include <BLUA/BLUAmanager.h>
#include <CFG/CFGmap.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
#include <COL/COLweb.h>
COL_LOG_MODULE;

static void NOTIenabledImp(CFGmap* pMap, NOTImodel* pNoti, BLUAinstanceManager* pManager, COLvar* pResult) {
   COL_FUNCTION(NOTIenabledImp);
   COLvar& Data = *pResult;
   COLstring ComponentId = pNoti->ComponentId;
   COL_VAR(ComponentId);
   Data["exists"] = true;
   if (ComponentId == "DISABLED") {
      COL_TRC("Notifications are disabled");
      Data["enabled"] = false;
   }
   else if (pMap->ConfigMap.count(ComponentId) > 0 && pManager->m_Instances.count(ComponentId) > 0) {
      COL_TRC("Component exists and is running");
      Data["enabled"] = true;
   }
   else if (pMap->ConfigMap.count(ComponentId) > 0) {
      COL_TRC("Component exists and but isn't running");
      Data["enabled"] = false;
   }
   else {
      COL_TRC("Component id does not exist");
      Data["enabled"] = false;
      Data["exists"] = false;
   }
   COL_VAR(Data.json(1));
}

// /notifications/enabled
void NOTIenabled(const COLwebRequest& Request, NOTImodel* pNoti, CFGmap* pMap, BLUAinstanceManager* pManager) {
   COL_FUNCTION(NOTIenabled);
   COLvar Result;
   NOTIenabledImp(pMap, pNoti, pManager, &Result);
   COLrespondSuccess(Request.Address, Result);
}