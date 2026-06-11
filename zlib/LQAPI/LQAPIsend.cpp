//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LQAPIsend
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Tues, June 7th, 2021
//---------------------------------------------------------------------------
#include "LQAPIsend.h"

#include <BLUA/BLUAcall.h>
#include <BLUA/BLUAmanager.h>

#include <CFG/CFGmap.h>

#include <LUA/LUAerror.h>
#include <LUA/LUAmessage.h>
#include <LUA/LUAtable.h>
#include <LUA/LUAutils.h>

#include <SCK/SCKloop.h>

#include <COL/COLclosure6.h>
#include <COL/COLevent.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#define LQAPI_MESSAGE_SEND "LQAPI_MESSAGE_SEND"
#define LQAPI_MESSAGE_SEND_META "LQAPI_MESSAGE_SEND_META"

enum LQAPImessageSendResult {
   LQAPI_SEND_SUCCESS = 0,
   LQAPI_TARGET_NOT_FOUND,
   LQAPI_TARGET_NOT_RUNNING,
};

// message.send{data, id} or message.send(data, id)
int LQAPIpushTransient(lua_State* L){
   COL_FUNCTION(LQAPIpushTransient);
   COLvar* pData = new COLvar();
   if (lua_istable(L, 1)) { // message.send{data="MSH", id="j3o2fkhfewfeqw"}
      pData->fromLua(L, -1);
      if (!pData->exists("data"))   { delete pData; LUA_ERROR_STREAM ("Parameter 'data' is required."); }
      if (!pData->exists("id")) {
         if (pData->exists("target")){ (*pData)["id"] = (*pData)["target"]; } // compatibility - accept "target" in place of id
         else                        { delete pData; LUA_ERROR_STREAM ("String parameter 'id' is required."); }
      }
   } else {
      (*pData)["data"].fromLua(L, 1);
      (*pData)["id"] = LUAtoString(L, 2);
   }
   COLstring TargetId = (*pData)["id"];
   lua_getfield(L, LUA_REGISTRYINDEX, LQAPI_MESSAGE_SEND);
   COLclosure3<COLvar*, COLstring, int*>* pFunction = *(COLclosure3<COLvar*, COLstring, int*>**)lua_touserdata(L, -1);
   COL_VAR(pFunction);
   if (!pFunction) {
      COL_WRN("message.send is not set up.");
      delete pData;
      luaL_error(L, "Internal Error: The API is not set up properly.");
      return 0;
   }
   int Result = 0;
   pFunction->run(pData, LUAgetMessageId(L), &Result);
   if (Result) {
      COLstring Error;
      switch (Result) {
      case LQAPI_TARGET_NOT_FOUND:   Error = "Target component '" + TargetId + "' does not exist.";                                             break;
      case LQAPI_TARGET_NOT_RUNNING: Error = "Target component '" + TargetId + "' is not running.";                                             break;
      default:                       Error = "Failed to send message to target component '" + TargetId + "'. Code " + COLintToString(Result);   break;
      }
      COL_ERR("message.send failed with error: " << Error);
      return luaL_error(L, Error.c_str());
   }
   return 0;
}

static void LQAPImessageSent(COLvar Result) {}

static void LQAPIsendMessage(COLvar* pData, COLstring RelatedId, int* pResult, CFGmap* pMap, BLUAinstanceManager* pBluaManager, LUAmode Mode) {
   COLstring TargetId = (*pData)["id"];
   if (pMap->ConfigMap.count(TargetId) == 0) {
      COL_TRC("Target component doesn't exist.");
      *pResult = LQAPI_TARGET_NOT_FOUND;
   }
   else if (LUA_PRODUCTION_MODE == Mode) {
      if (!BLUAinstanceRunning(*pBluaManager, TargetId)) {
         COL_TRC("Target component not running.");
         *pResult = LQAPI_TARGET_NOT_RUNNING;
      } else {
         COL_TRC("Sending message for production mode.");
         COLclosure1<COLvar>* pDone = COLnewClosure1(&LQAPImessageSent);
         BLUAmessageIdCall(pBluaManager->m_Instances[TargetId], (*pData)["data"], RelatedId, pDone);
         *pResult = LQAPI_SEND_SUCCESS;
      }
   }
   else {   // annotation or intellisense mode
      COL_TRC("No-op for annotation and intellisense mode.");
      *pResult = LQAPI_SEND_SUCCESS;
   }
   delete pData;
}

// MAIN THREAD
static void LQAPImessageSendOnMain(COLclosure0* pSender, COLevent* pEvent) {
   COL_FUNCTION(LQAPImessageSendOnMain);
   pSender->runAndDelete();
   COL_TRC("Signal LQAPImessageSendFromLua() to continue");
   pEvent->signal();
}

// LUA THREAD
static void LQAPImessageSendFromLua(COLvar* pData, COLstring RelatedId, int* pResult, LUAmode Mode, CFGmap* pMap, BLUAinstanceManager* pBluaManager, SCKloop* pLoop) {
   COL_FUNCTION(LQAPImessageSendFromLua);
   COLevent Event;
   // ran out of closure arguments (limit is 6, need 7), so pSender is introduced
   COLclosure0* pSender = COLnewClosure0(&LQAPIsendMessage, pData, RelatedId, pResult, pMap, pBluaManager, Mode);
   pLoop->post(COLnewClosure0(&LQAPImessageSendOnMain, pSender, &Event));
   COL_TRC("Block until LQAPImessageSendOnMain() signals");
   Event.wait();
}

void LQAPIsetUpMessageSend(lua_State* L, LUAmode Mode, CFGmap* pMap, BLUAinstanceManager* pBluaManager, SCKloop* pLoop) {
   COL_FUNCTION(LQAPIsetUpMessageSend);
   // create message.send closure
   COLclosure3<COLvar*, COLstring, int*>* pFunction = COLnewClosure3(&LQAPImessageSendFromLua, Mode, pMap, pBluaManager, pLoop);
   COL_VAR(pFunction);
   // wrap closure in lua user data to automatically free it through garbage collection
   LUApushPointer<COLclosure3<COLvar*, COLstring, int*>*>(L, pFunction, LQAPI_MESSAGE_SEND_META);
   lua_setfield(L, LUA_REGISTRYINDEX, LQAPI_MESSAGE_SEND);
}