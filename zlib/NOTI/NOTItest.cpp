// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: NOTItest
//
//  Description: test notification config
//
//  Author: Matthew Sobkowski
//  Date:   8/31/22, 10:53 AM
// ---------------------------------------------------------------------------
#include "NOTItest.h"

#include <BLUA/BLUAmanager.h>
#include <NOTI/NOTImodel.h>
#include <NOTI/NOTIdetect.h>
#include <SCK/SCKloop.h>
#include <COL/COLlog.h>
#include <COL/COLweb.h>
COL_LOG_MODULE;

// /notifications/test
void NOTItest(const COLwebRequest& Request, NOTImodel* pNoti, CFGmap* pMap, BLUAinstanceManager* pManager) {
   COL_FUNCTION(NOTItest);
   if (pNoti->ComponentId == "DISABLED") {
      return COLrespondError(Request.Address, "Notification component not configured.");
   }
   if (!BLUAinstanceRunning(*pManager, pNoti->ComponentId)) {
      return COLrespondError(Request.Address, "Please start the notification component before running a test.");
   }
   int Count = NOTIdetectAll(pMap, pNoti, pManager);
   COL_VAR(Count);
   if (Count == 0){
      return COLrespondSuccess(Request.Address, "No rules were triggered, so no action was taken.");
   } else {
      return COLrespondSuccess(Request.Address, "Notification rules have been triggered.  Please look at the logs for the notification component for details.");
   }
}