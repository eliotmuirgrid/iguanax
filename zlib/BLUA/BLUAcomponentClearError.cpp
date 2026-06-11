// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAcomponentClearError
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Thursday 13 April 2023 - 01:05PM
// ---------------------------------------------------------------------------

#include <BLUA/BLUAcomponentClearError.h>
#include <BLUA/BLUAmanager.h>
#include <BLUA/BLUAutils.h>
#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>
#include <PRM/PRMcallbacks.h>
#include <USR/USRuser.h>
#include <COL/COLvarUtils.h>
#include <COL/COLlog.h>
#include <COL/COLweb.h>
COL_LOG_MODULE;

//component/clear_error
void BLUAcomponentClearError(const COLwebRequest& Request, BLUAinstanceManager* pManager, CFGmap* pMap) {
   COL_FUNCTION(BLUAcomponentClearError);
   COLvar ComponentIds = COLvarRequiredObject(Request.Data, "components");
   COLvar Result;
   Result["cleared"].setArrayType();
   Result["skipped"].setArrayType();
   Result["errors"].setArrayType();
   COLstring Agent = Request.User;
   COLstring Error;
   COLarray<COLstring> GoodComponents;
   COLarray<COLstring> BadComponents;
   CFGconvertNameToIdIfNeeded(ComponentIds, pMap, &GoodComponents, &BadComponents);
   for (const auto& BadComponent : BadComponents) {
      Result["skipped"].push_back(BadComponent);
      Result["errors"].push_back("No component exists with name or id " + BadComponent);
   }
   for(int i = 0; i < GoodComponents.size(); i++){
      const COLstring Guid = GoodComponents[i];
      CFGconfig* pTransNode = CFGmapNode(pMap, Guid);
      COL_VAR(*pTransNode);
      if(pTransNode) {
         if(!PRMcheckRoleAccess(Request.FromTranslator, Agent, pTransNode->CoreConfig.Guid, ROLid::CLEAR_ERRORS, &Error)){
            Result["skipped"].push_back(Guid);
            Result["errors"].push_back(PRMsetError(ROLid::CLEAR_ERRORS));
            continue;
         }
         pTransNode->Status.ErrorCount = 0;
         if (pTransNode->Status.Light == CFG_LIGHT::ERR){
            if (pManager->m_Instances.count(pTransNode->CoreConfig.Guid)){
               COL_TRC("Component is running so change light to green.");
               pTransNode->Status.Light = CFG_LIGHT::ON;
            } else {
               COL_TRC("Component is off so change light to off.");
               pTransNode->Status.Light = CFG_LIGHT::OFF;
            }
         } 
         Result["cleared"].push_back(Guid);
      }
   }
   COLrespondSuccess(Request.Address, Result);
}
