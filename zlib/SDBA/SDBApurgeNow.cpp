// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBApurgeNow
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Thursday 05 October 2023 - 04:21PM
// ---------------------------------------------------------------------------

#include <SDBA/SDBApurgeNow.h>
#include <SDB/SDBcapi.h>
#include <SDB/SDBpurgeAuto.h>
#include <SDB/SDBsystem.h>

#include <PRM/PRMcallbacks.h>

#include <ROL/ROLid.h>

#include <SCK/SCKloop.h>

#include <COL/COLclosure6.h>
#include <COL/COLthreadPool.h>
#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void SDBApurgeNowDone(COLaddress Address){
   COL_FUNCTION(SDBApurgeNowDone);
   COLrespondSuccess(Address);
}

// /log/purge_rules/purge
void SDBApurgeNow(const COLwebRequest& Request, SDBsystem* pSystem){
   COL_FUNCTION(SDBApurgeNow);
   COLstring Error;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::LOG_PURGING, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::LOG_PURGING));
   }

   COLvar Result;
   COLhashmap<COLstring, SDBpurgeData> PurgeInfo;
   COLhashmap<COLstring, bool> AllComponents;
   SDBpopulatePurgeInfo(&PurgeInfo, pSystem);
   SDBpopulateAllComponentMap(&AllComponents, pSystem);
   COLclosure0* pDone = COLnewClosure0(&SDBApurgeNowDone, Request.Address);
   pSystem->pLoop->threadPool()->scheduleTask(COLnewClosure0(SDBpurgeLogsWorker, pSystem->pLoop, pSystem->LogPurgeRules, PurgeInfo, AllComponents, pDone));
   SDBIlogService("User '" + Request.User + "' manually started log purging.", "#purge");
}