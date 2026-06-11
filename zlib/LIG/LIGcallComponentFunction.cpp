// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LIGcallComponentFunction
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Monday 21 April 2025 - 03:09PM
// ---------------------------------------------------------------------------
#include <LIG/LIGcallComponentFunction.h>

#include <BLUA/BLUAcall.h>
#include <BLUA/BLUAmanager.h>
#include <BLUA/BLUAstart.h>

#include <LUA/LUAerror.h>
#include <LUA/LUAtable.h>
#include <LUA/LUAutils.h>

#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>

#include <SCK/SCKloop.h>

#include <COL/COLclosure.h>
#include <COL/COLevent.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

#define LIG_COMPONENT_CALL_META "LIG_COMPONENT_CALL_METATABLE"
#define LIG_COMPONENT_CALL "LIG_COMPONENT_CALL"

static void LIGpopulateComponentCallPayload(COLvar* pPayload, const COLstring& Name, const COLstring& Data, const COLstring& Function, bool Async){
   COL_FUNCTION(LIGpopulateComponentCallPayload);
   (*pPayload)["name"]  = Name;
   (*pPayload)["func"]  = Function;
   (*pPayload)["data"]  = Data;
   (*pPayload)["async"] = Async;
}

static void LIGhandleComponentCallResult(lua_State* L, COLauto<COLvar> pResult, int* pNumResults){
   COL_FUNCTION(LIGhandleComponentCallResult);
   if (pResult->exists("error")){
      luaL_error(L, (*pResult)["error"].asString().c_str());
      return;
   }
   if (!pResult->isArray()){
      luaL_error(L, "Internal Error: The API did not receive data in the expected format.");
      return;
   }
   *pNumResults = pResult->size();
   for (int i = 0; i < pResult->size(); i++){
      (*pResult)[i].toLua(L);
   }
}

// component.call{id="Target_2sQqAbqfNEqcdU", data=Data(must be a string), func="main", async(optional)=bool}
int LIGcallComponentFunction(lua_State* L){
   COL_FUNCTION(LIGcallComponentFunction);
   COLstring Data     = LUAexpectedStringParam(L, 1, "data");
   COLstring Function = LUAexpectedStringParam(L, 1, "func");
   COLstring Name     = LUAexpectedStringParam(L, 1, "name");
   bool      Async    = LUAoptionalBoolParam(L, 1, "async", false);
   COLvar* pPayload = new COLvar();
   LIGpopulateComponentCallPayload(pPayload, Name, Data, Function, Async);
   COL_VAR(*pPayload);
   lua_getfield(L, LUA_REGISTRYINDEX, LIG_COMPONENT_CALL); // This is a closure set up when the lua state is created. It gets cleaned up by the lua garbage collector.
   COLclosure2<COLvar*, COLvar*>* pFunction = *(COLclosure2<COLvar*, COLvar*>**)lua_touserdata(L, -1);
   COL_VAR(pFunction);
   if (!pFunction) {
      COL_WRN("iguana.callComponentFunction is not set up.");
      delete pPayload;
      return luaL_error(L, "Internal Error: The API is not set up properly.");
   }
   COLauto<COLvar> pResult = new COLvar();
   pFunction->run(pPayload, pResult);
   COL_VAR2(*pResult, pResult->size());
   int NumResults = 0;
   LIGhandleComponentCallResult(L, pResult, &NumResults);
   return NumResults;
}

// MAIN THREAD
void LIGcomponentCallOnMain(COLvar* pData, CFGmap* pMap, BLUAinstanceManager* pManager, SCKloop* pLoop, COLclosure1<COLvar>* pCallback) {
   COL_FUNCTION(LIGcomponentCallOnMain);
   COLstring Target = (*pData)["name"];
   CFGconfig* pConfig = CFGmapNode(pMap, Target);
   COLvar Out;
   if(!pConfig) {
      COL_TRC("Target component doesn't exist.");
      Out["error"] = "Target component doesn't exist.";
      pCallback->runAndDelete(Out); // cleans up pCallback
      return;
   }
   COLstring TargetId = pConfig->CoreConfig.Guid;
   // Make sure the target component exists and running
   if (pMap->ConfigMap.count(TargetId) == 0) {
      COL_TRC("Target component doesn't exist.");
      Out["error"] = "Target component doesn't exist.";
      pCallback->runAndDelete(Out); // cleans up pCallback
      return;
   }
   if (!BLUAinstanceRunning(*pManager, TargetId)) {
      if (pData->exists("autostart") && (*pData)["autostart"].asBool()) {
         COL_TRC("Autostart target component.");
         BLUAinstanceStart(pManager, pConfig, pLoop);
      } else {
         COL_TRC("Target component not running.");
         Out["error"] = "Target component not running.";
         pCallback->runAndDelete(Out);
         return;
      }
   }
   // If async, we don't need to wait for callback
   if ((*pData)["async"].asBool()) {
      COL_TRC("Trying to call " << (*pData)["name"] << "'s function " << (*pData)["func"]);
      BLUAcallFunction(pManager->m_Instances[TargetId], (*pData)["func"], (*pData)["data"]);
      Out.setArrayType(); // return an empty array for an async call
      pCallback->runAndDelete(Out);
   } else {
      COL_TRC("Trying to call " << (*pData)["name"] << "'s function " << (*pData)["func"]);
      BLUAcallbackCall(pManager->m_Instances[TargetId], (*pData)["func"], (*pData)["data"], pCallback); // BLUAreportCallResult will run and delete this callback
   }
   // Always want to clean up the data pointer
   delete pData;
}

static void LIGcomponentCallDone(COLvar BluaResult, COLevent* pEvent, COLvar* pResult){
   COL_FUNCTION(LIGcomponentCallDone);
   if (BluaResult.exists("error")) { COL_TRC("The function being called by component.call failed with the error:\n" + BluaResult["error"].str()); }
   *pResult = BluaResult;  // if doing an async call, BluaResult will be an empty array
   pEvent->signal();
}

// LUA THREAD
static void LIGcomponentCallFromLua(COLvar* pData, COLvar* pResult, CFGmap* pMap, BLUAinstanceManager* pManager, SCKloop* pLoop) {
   COL_FUNCTION(LIGcomponentCallFromLua);
   COLevent Event;
   COLclosure1<COLvar>* pCallback = COLnewClosure1(&LIGcomponentCallDone, &Event, pResult); // cleaned up by BLUAreportCallResult
   pLoop->post(COLnewClosure0(&LIGcomponentCallOnMain, pData, pMap, pManager, pLoop, pCallback));  // post to main thread since we're modifying BLUAmanager
   COL_TRC("Block until LIGcomponentCallDone() signals");
   Event.wait();
}

void LIGsetUpComponentCall(lua_State* L, CFGmap* pMap, BLUAinstanceManager* pBluaManager, SCKloop* pLoop) {
   COL_FUNCTION(LIGsetUpComponentCall);
   // create component.call closure
   COLclosure2<COLvar*, COLvar*>* pFunction = COLnewClosure2(&LIGcomponentCallFromLua, pMap, pBluaManager, pLoop);
   COL_VAR(pFunction);
   // wrap closure in lua user data to automatically free it through garbage collection
   LUApushPointer< COLclosure2<COLvar*, COLvar*>*>(L, pFunction, LIG_COMPONENT_CALL_META);
   lua_setfield(L, LUA_REGISTRYINDEX, LIG_COMPONENT_CALL);
}