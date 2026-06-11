//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: TRNCclearQueue
//
// Author: John Q
//
// Implementation
//---------------------------------------------------------------------------
#include <CFG/CFGconfig.h>
#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <PRM/PRMcallbacks.h>
#include <SDB/SDBcapi.h>
#include <TRNC/TRNCclearQueue.h>
COL_LOG_MODULE;

// /component/clear_queue
void TRNCcomponentClearQueue(const COLwebRequest& Request, CFGmap* pMap) {
   COL_FUNCTION(TRNCcomponentClearQueue);
   COL_VAR(Request.Data.json(1));
   const COLvar ComponentIds = COLvarRequiredStringOrStringList(Request.Data, "components");
   COL_VAR(ComponentIds);
   COLvar Result;
   Result["cleared"].setArrayType();
   Result["skipped"].setArrayType();
   Result["errors"].setArrayType();
   COLstring Error;
   COLarray<COLstring> GoodComponents;
   COLarray<COLstring> BadComponents;
   CFGconvertNameToIdIfNeeded(ComponentIds, pMap, &GoodComponents, &BadComponents);
   for (const auto& BadComponent : BadComponents) {
      Result["skipped"].push_back(BadComponent);
      Result["errors"].push_back("No component exists with name or id " + BadComponent);
   }
   for(const auto& Component : GoodComponents) {
      const CFGconfig* pTransNode = CFGmapNode(pMap, Component);
      if(pTransNode) {
         if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, Component, CLEAR_QUEUE, &Error)) {
            Result["errors"].push_back(Component);
            continue;
         }
         COL_TRC("Clearing component queue " << Component);
         SDBIlogComponent("Accepted clear queue request by user '" + Request.User + "'.", Component);
         SDBIclearQueueCount(Component);
         Result["cleared"].push_back(Component);
      }
   }
   COLrespondSuccess(Request.Address, Result);
}
