//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAcallCallback
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Friday Sept 30 2022
//---------------------------------------------------------------------------

#include <SCK/SCKloop.h>

#include <LUA/LUAutils.h>

#include <BLUA/BLUAtask.h>
#include <BLUA/BLUAinstance.h>

#include <COL/COLvarUtils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


BLUAtask BLUAcallCallbackTask(const COLstring& FuncName, const COLvar& Data) {
   COL_FUNCTION(BLUAcallCallbackTask);
   BLUAtask Task;
   Task.Id = BLUA_ON_CALL;
   Task.Payload["data"] = Data;
   Task.Payload["fcn"]  = FuncName;
   // store callback in metadata
   Task.Payload["meta"]["cb"] = true;
   return Task;
}

static void BLUAreportCallResult(lua_State* L, const COLvar& Result) {
   COL_FUNCTION(BLUAreportCallResult);
   BLUAinstance2* pInstance = BLUAinstanceGet(L);
   COLASSERT(pInstance->m_CallbackQueue.size());
   COLclosure1<COLvar>* pCallback = pInstance->m_CallbackQueue.take();
   pInstance->m_pLoop->post(COLnewClosure0(pCallback, &COLclosure1<COLvar>::runAndDelete, Result));
}

static void BLUAhandleFunctionReturns(lua_State* L, COLvar* pReturnVal){
   COL_FUNCTION(BLUAhandleFunctionReturns);
   int NumReturns = lua_gettop(L);
   pReturnVal->setArrayType();
   for (int i = 1; i <= NumReturns; i++){
      COLvar Temp;
      Temp.fromLua(L, i);
      pReturnVal->push_back(Temp);
   }
   COL_TRC("Got " << pReturnVal->size() << " results");
   lua_pop(L, NumReturns); // Clean up stack
}

// LUA THREAD
void BLUAhandleCallbackCall(lua_State* L, const COLvar& Payload, int* pResult) {
   COL_FUNCTION(BLUAhandleCallbackCall);
   COL_VAR(Payload);
   COLvar ReturnVal;
   COLstring FunctionName = COLvarGet(Payload, "fcn", "main");
   if (!BLUAgetCalledFunction(FunctionName, L, pResult)) {
      ReturnVal["error"] = "Function '" + FunctionName + "' is not found.";
   } else {
      Payload["data"].toLua(L);
      *pResult = lua_pcall(L, 1, LUA_MULTRET, 0);
      if (*pResult) {
         ReturnVal["error"] = LUAtoString(L, -1);
      } else {
         BLUAhandleFunctionReturns(L, &ReturnVal);
      }
   }
   BLUAreportCallResult(L, ReturnVal);
}

void BLUAcallbackCall(BLUAinstance2* pInstance, const COLstring& FuncName, const COLvar& Data, COLclosure1<COLvar>* pCallback) {
   COL_FUNCTION(BLUAcallbackCall);
   pInstance->m_CallbackQueue.put(pCallback);
   pInstance->m_JobQueue.put(BLUAcallCallbackTask(FuncName, Data));
}
