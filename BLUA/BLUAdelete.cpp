//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAdelete
//
// Description:
//
// Implementation
//
// Author: Vismay Shah refactored by Nits
// Date:   Friday May 14 2021
//---------------------------------------------------------------------------
#include <BLUA/BLUAdelete.h>
#include <BLUA/BLUAmanager.h>
#include <CFG/CFGconfig.h>
#include <CFG/CFGmapApi.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DELC/DELCdeleteComponent.h>
#include <PRM/PRMcallbacks.h>
#include <SDB/SDBserviceLog.h>
COL_LOG_MODULE;

// MAIN THREAD
static void BLUAcleanupDone(COLaddress Address, COLvar ResponseVar) {
   COL_FUNCTION(BLUAcleanupDone);
   // If Result is an error var in theory it should be okay - Eliot
   COL_VAR(ResponseVar);
   COLrespondSuccess(Address, ResponseVar);
}

static void BLUAupdateDeletionResults(BLUAinstanceManager* pBluaManager, const CFGconfig& Config, COLvar* pResult,
                                      COLarray<COLstring>* pToDelete) {
   COL_FUNCTION(BLUAupdateDeletionResults);
   const COLstring Key = BLUAinstanceRunning(*pBluaManager, Config.CoreConfig.Guid) ? "skipped" : "removed";
   COLvar          Data;
   Data["name"] = Config.CoreConfig.Name;
   Data["guid"] = Config.CoreConfig.Guid;
   (*pResult)[Key].push_back(Data);
   if(Key == "removed") { pToDelete->push_back(Config.CoreConfig.Guid); }
}

static void BLUAprocessSingleComponent(const CFGconfig& Config, BLUAinstanceManager* pBluaManager,
                                       const bool FromTranslator, const COLstring& User, COLvar* pResult,
                                       COLarray<COLstring>* pToDelete) {
   COL_FUNCTION(BLUAprocessSingleComponent);
   COLstring Error;
   if(PRMcheckRoleAccess(FromTranslator, User, Config.CoreConfig.Guid, DELETE_COMPONENT, &Error)) {
      BLUAupdateDeletionResults(pBluaManager, Config, pResult, pToDelete);
   } else {
      COL_TRC("Missing permission to remove " << Config.CoreConfig.Guid);
      (*pResult)["skipped"].push_back(Config.CoreConfig.Guid);
      (*pResult)["errors"].push_back(PRMsetError(DELETE_COMPONENT));
   }
}
static bool BLUAisValidComponent(const COLstring& Component, CFGmap* pMap, CFGconfig** pConfig) {
   COL_FUNCTION(BLUAisValidComponent);
   COL_VAR(Component);
   if(Component.is_null() || Component.isWhitespace() || Component == SDB_SERVICE_COMPONENT_ID) return false;
   *pConfig = CFGmapNode(pMap, Component);
   return *pConfig != nullptr;
}

static void BLUAdeleteComponentImpl(const COLvar& ToRemove, const COLstring& User, CFGmap* pMap,
                                    BLUAinstanceManager* pBluaManager, bool FromTranslator, const COLaddress& Address) {
   COL_FUNCTION(BLUAdeleteComponentImpl);
   COLvar Result;
   Result["removed"].setArrayType();
   Result["skipped"].setArrayType();
   COLarray<COLstring> GuidsToDelete;
   for(const auto& Component : ToRemove.array()) {
      CFGconfig* pConfig = nullptr;
      if(BLUAisValidComponent(Component, pMap, &pConfig)) {
         BLUAprocessSingleComponent(*pConfig, pBluaManager, FromTranslator, User, &Result, &GuidsToDelete);
      }
   }
   COL_VAR(Result.json(1));
   DELCrunComponentDelete(GuidsToDelete, User, COLnewClosure0(&BLUAcleanupDone, Address, Result));
}

// /component/delete
void BLUAdeleteComponent(const COLwebRequest& Request, BLUAinstanceManager* pBluaManager, CFGmap* pMap) {
   COL_FUNCTION(BLUAdeleteComponent);
   COL_VAR(Request.Data);
   const COLvar& Components = COLvarRequiredStringOrStringList(Request.Data, "components");
   if(!Components.isArray()) {
      COL_TRC("Invalid ToRemove object");
      return COLrespondError(Request.Address, "Invalid input object.");
   }
   BLUAdeleteComponentImpl(Components, Request.User, pMap, pBluaManager, Request.FromTranslator, Request.Address);
}
