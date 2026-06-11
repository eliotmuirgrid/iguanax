// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAcomponentFunctionCall
//
// Description:
// Generic component function call web API endpoint implementation.
//
// Author: iNTERFACEWARE Inc.
// ---------------------------------------------------------------------------
#include <BLUA/BLUAcomponentFunctionCall.h>
#include <BLUA/BLUAstart.h>
#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>
#include <COL/COLclosure.h>
#include <COL/COLweb.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <LIG/LIGcallComponentFunction.h>
#include <BLUA/BLUAcall.h>
#include <BLUA/BLUAmanager.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
COL_LOG_MODULE;

static bool BLUAcheckAccess(const COLwebRequest& Request, const COLstring& ComponentGuid, COLstring* pError){
   COLstring Error;
   do {
      if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, ComponentGuid, ROLid::START, &Error)) {
         *pError = PRMsetError(ROLid::START);
         break;
      }
      if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, ComponentGuid, ROLid::VIEW_SCRIPT, &Error)) {
         *pError = PRMsetError(ROLid::VIEW_SCRIPT);
         break;
      }
      if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, ComponentGuid, ROLid::EDIT_SCRIPT, &Error)) {
         *pError = PRMsetError(ROLid::EDIT_SCRIPT);
         break;
      }
      return true;
   } while (false);
   return false;
}

static void BLUArespondFunctionCallResult(COLvar CallbackResult, COLaddress Address){
   COL_FUNCTION(BLUArespondFunctionCallResult);
   if(CallbackResult.exists("error")) return COLrespondError(Address, CallbackResult["error"].asString());
   COLrespondSuccess(Address, CallbackResult);
}

// /component/function/call
void BLUAcomponentFunctionCall(const COLwebRequest& Request, BLUAinstanceManager* pManager, CFGmap* pMap, SCKloop* pLoop) {
   COL_FUNCTION(BLUAcomponentFunctionCall);
   const COLvar& Data = Request.Data;
   COL_VAR(Data);
   const COLstring& Component = COLvarRequiredString(Data, "component");
   const COLstring& Function  = COLvarRequiredString(Data, "function");
   const COLvar&    Argument  = COLvarRequiredObject(Data, "data");
   bool             Async     = COLvarGetBool       (Data, "async", false);
   bool             AutoStart = COLvarGetBool       (Data, "start_if_stopped", true);

   CFGconfig* pConfig = CFGmapNode(pMap, Component);
   if (!pConfig) { return COLrespondError(Request.Address, "Target component doesn't exist."); }
   COLstring Error;
   const COLstring& Guid = pConfig->CoreConfig.Guid;
   if (!BLUAcheckAccess(Request, Guid, &Error)) { return COLrespondError(Request.Address, Error); }

   COLvar* pData = new COLvar();
   (*pData)["name"]  = Component;
   (*pData)["func"]  = Function;
   (*pData)["data"]  = Argument;
   (*pData)["async"] = Async;
   (*pData)["autostart"] = AutoStart;
   COLclosure1<COLvar>* pCallback = COLnewClosure1(&BLUArespondFunctionCallResult, Request.Address);
   LIGcomponentCallOnMain(pData, pMap, pManager, pLoop, pCallback);
}
