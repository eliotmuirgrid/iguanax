//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAcallMessageId
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Friday Sept 30 2022
//---------------------------------------------------------------------------

#include <LUA/LUAutils.h>
#include <LUA/LUAmessage.h>

#include <SCK/SCKloop.h>

#include <BLUA/BLUAtask.h>
#include <BLUA/BLUAinstance.h>
#include <BLUA/BLUAcall.h>

#include <COL/COLvarUtils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


BLUAtask BLUAcallMessageIdTask(const COLvar& Data, const COLstring& RelatedId) {
   COL_FUNCTION(BLUAcallMessageIdTask);
   BLUAtask Task;
   Task.Id = BLUA_ON_CALL;
   Task.Payload["data"] = Data;
   Task.Payload["fcn"]  = "main";
   Task.Payload["meta"]["msg_id"] = RelatedId;
   return Task;
}

static void BLUAsetMessageId(lua_State* L, const COLstring& RelatedId) {
   COL_FUNCTION(BLUAsetMessageId);
   if (RelatedId.is_null()) { return; }
   COL_VAR("Set message id " << RelatedId);
   COLstring Source;
   COLuint64 DateTime, Index;
   LUAparseMessageId(RelatedId, &Source, &DateTime, &Index);
   LUAsetMessage(L, Source, DateTime, Index);
}

static void BLUArunCallback(lua_State* L) {
   COL_FUNCTION(BLUArunCallback);
   BLUAinstance2* pInstance = BLUAinstanceGet(L);
   COLASSERT(pInstance->m_CallbackQueue.size());
   COLclosure1<COLvar>* pCallback = pInstance->m_CallbackQueue.take();
   pInstance->m_pLoop->post(COLnewClosure0(pCallback, &COLclosure1<COLvar>::runAndDelete, COLvar()));
}

// LUA THREAD
void BLUAhandleMessageIdCall(lua_State* L, const COLvar& Payload, int* pResult) {
   COL_FUNCTION(BLUAhandleMessageIdCall);
   COLstring FunctionName = COLvarGet(Payload, "fcn", "main");
   if (!BLUAgetCalledFunction(FunctionName, L, pResult)) {
      COL_TRC("No " << FunctionName << " function found.");
      return;
   }
   Payload["data"].toLua(L);
   BLUAsetMessageId(L, Payload["meta"]["msg_id"]);
   *pResult = lua_pcall(L, 1, 0, 0);
   // message.send could be called to send data to a source component or a destination component.
   // Source components have a nil message id while destination components set their message id
   // (in BLUAhandleDequeue()) every time a messages is dequeued.
   // We don't need to worry about destination components, but we do need to nullify the message
   // id for source components. Resetting it to null here should take care of both cases.
   LUAsetupDefaultMessage(L);
   BLUArunCallback(L);
}

void BLUAmessageIdCall(BLUAinstance2* pInstance, const COLvar& Data, const COLstring& RelatedId, COLclosure1<COLvar>* pCallback) {
   COL_FUNCTION(BLUAmessageIdCall);
   pInstance->m_CallbackQueue.put(pCallback);
   pInstance->m_JobQueue.put(BLUAcallMessageIdTask(Data, RelatedId));
}
