//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: BLUAstopAll.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   11/01/24 12:43 PM
//  ---------------------------------------------------------------------------
#include <BLUA/BLUAstopAll.h>
#include <BLUA/BLUAcore.h>
#include <BLUA/BLUAmanager.h>
#include <BLUA/BLUAinstance.h>
#include <BLUA/BLUAkill.h>
#include <BLUA/BLUAstop.h>

#include <CFG/CFGconfig.h>

#include <PRM/PRMcallbacks.h>

#include <ROL/ROLid.h>

#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
COL_LOG_MODULE;

// /component/stop_all
void BLUAcomponentStopAll(const COLwebRequest& Request, BLUAinstanceManager* pManager) {
   COL_FUNCTION(BLUAcomponentStopAll);
   COLvar Result;
   COLstring Error;
   for(const auto& it : pManager->m_Instances) {
      if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, it.first, ROLid::STOP, &Error)){
         Result["skipped"].push_back(it.first);
         Result["errors"].push_back(PRMsetError(ROLid::STOP));
         continue;
      }
      COL_TRC("Shutting down " << it->first);
      it->second->m_Config.Status.Light = CFG_LIGHT::IN_PROGRESS;
      it->second->m_IsStopping = true;
      BLUAinstanceDelete(it->second, true);
   }
   COLrespondSuccess(Request.Address, Result);
}
