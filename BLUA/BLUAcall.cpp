//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAcall
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Friday Sept 30 2022
//---------------------------------------------------------------------------

#include <LUA/LUAutils.h>

#include <BLUA/BLUAtask.h>
#include <BLUA/BLUAinstance.h>
#include <BLUA/BLUAcall.h>
#include <BLUA/BLUAcore.h>

#include <COL/COLvarUtils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


BLUAtask BLUAcallTask(const COLstring& FuncName, const COLvar& Data) {
   COL_FUNCTION(BLUAcallTask);
   BLUAtask Task;
   Task.Id = BLUA_ON_CALL;
   Task.Payload["data"] = Data;
   Task.Payload["fcn"]  = FuncName;
   return Task;
}

// LUA THREAD
static void BLUAhandleStandardCall(lua_State* L, const COLvar& Payload, int* pResult) {
   COL_FUNCTION(BLUAhandleStandardCall);
   COLstring FunctionName = COLvarGet(Payload, "fcn", "main");
   if (!BLUAgetCalledFunction(FunctionName, L, pResult)) {
      COL_TRC("No " << FunctionName << " function found.");
      return;
   }
   Payload["data"].toLua(L);
   *pResult = lua_pcall(L, 1, 0, 0);
   BLUAprintThreadId(L);
}

void BLUAhandleCall(lua_State*& L, const COLvar& Payload, int* pResult) {
   COL_FUNCTION(BLUAhandleCall);
   COLASSERT(L);
   if (!Payload.exists("meta")) {
      COL_TRC("We have a standard call");
      BLUAhandleStandardCall(L, Payload, pResult);
      return;
   }
   if (Payload["meta"].exists("msg_id")) {
      COL_TRC("We have a message.send call");
      BLUAhandleMessageIdCall(L, Payload, pResult);
      return;
   }
   if (Payload["meta"]["cb"].asBool()) {
      COL_TRC("We have a external auth call");
      BLUAhandleCallbackCall(L, Payload, pResult);
      return;
   }
}

void BLUAcallFunction(BLUAinstance2* pInstance, const COLstring& FuncName, const COLvar& Data) {
   COL_FUNCTION(BLUAcallFunction);
   pInstance->m_JobQueue.put(BLUAcallTask(FuncName, Data));
}
