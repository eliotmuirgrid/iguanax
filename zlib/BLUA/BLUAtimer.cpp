//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAtimer
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Friday May 14 2021
//---------------------------------------------------------------------------
#include "BLUAtimer.h"

#include <BLUA/BLUAinstance.h>
#include <BLUA/BLUAtask.h>
#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <GUID/GUIDid.h>
#include <LUA/LUAawait.h>
#include <LUA/LUAtable.h>
#include <LUA/LUAutils.h>
#include <LUA/LUAtest.h>
#include <LUAL/LUALutils.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

BLUAtask BLUAtimerTask(const COLstring& Data, int CallbackRef) {
   BLUAtask Task;
   Task.Id = BLUA_ON_TIMER;
   Task.Payload["data"]   = Data;
   Task.Payload["cb_ref"] = CallbackRef;
   return Task;
}

// LUA THREAD
void BLUAhandleTimer(lua_State*& L, const COLvar& Payload, int* pResult) {
   COL_FUNCTION(BLUAhandleTimer);
   COLASSERT(L);
   int FunctionRef = COLvarRequiredInt(Payload, "cb_ref");
   lua_rawgeti  (L, LUA_REGISTRYINDEX, FunctionRef);
   LUApushString(L, Payload["data"]);
   *pResult = lua_pcall(L, 1, 0, 0);
   COL_TRC("After calling time callback: " << *pResult);
}
