#ifndef __BLUA_CALL_H__
#define __BLUA_CALL_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAinstanceCall
//
// Description:
//    BLUA call task
//
// Author: John Q
// Date:   Friday Sept 30 2022
//---------------------------------------------------------------------------
#include <COL/COLvar.h>
#include <COL/COLstring.h>
#include <COL/COLclosure.h>

class  BLUAinstance2;
class  COLvar;
struct BLUAtask;
struct lua_State;

// call from main thread. caller's responsibility to ensure pInstance is already running

// standard call
BLUAtask BLUAcallTask(const COLstring& FuncName = "main", const COLvar& Data = "");
void BLUAhandleCall(lua_State*& L, const COLvar& Payload, int* pResult);
void BLUAcallFunction(BLUAinstance2* pInstance, const COLstring& FuncName = "main", const COLvar& Data = "");

// for DBDextAuth
BLUAtask BLUAcallCallbackTask(const COLstring& FuncName, const COLvar& Data);
void BLUAhandleCallbackCall(lua_State* L, const COLvar& Payload, int* pResult);
void BLUAcallbackCall(BLUAinstance2* pInstance, const COLstring& FuncName, const COLvar& Data, COLclosure1<COLvar>* pCallback);

// for LQAPIsend and SDBAresubmit
BLUAtask BLUAcallMessageIdTask(const COLvar& Data, const COLstring& RelatedId);
void BLUAhandleMessageIdCall(lua_State* L, const COLvar& Payload, int* pResult);
void BLUAmessageIdCall(BLUAinstance2* pInstance, const COLvar& Data, const COLstring& RelatedId, COLclosure1<COLvar>* pCallback);

#endif // end of defensive include
