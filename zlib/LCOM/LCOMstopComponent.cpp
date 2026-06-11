// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LCOMstopComponent
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Thursday 04 December 2025 - 10:33AM
// ---------------------------------------------------------------------------
#include <LCOM/LCOMstopComponent.h>

#include <LIG/LIGcall.h>
#include <LUA/LUAerror.h>
#include <LUA/LUAopen.h>
#include <LUA/LUAtest.h>
#include <SCK/SCKcall.h>
#include <SCK/SCKloop.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

int LCOMstopComponent(lua_State* L){
   COL_FUNCTION(LCOMstopComponent);
   if(LUALisEnvironmentTest(L)) {
      COL_TRC("In IDE mode, do nothing");
      return 0; 
   }
   const COLstring Component = LUAcomponentId(L);
   SCKloop* pLoop            = (SCKloop*)LUAgetPointerField(L, "_pEventLoop");
   COLvar Params;
   Params["components"] = Component;

   COLvar Result;
   COLstring Agent;
   bool ComponentAgent = true;
   LIGpopulateAgent(L, &Agent, ComponentAgent);
   bool StopSuccess = SCKcall("/component/stop", Params, Agent, &Result, ComponentAgent);
   COL_VAR(StopSuccess);
   if (!StopSuccess) {
      return LUAraiseLuaError(L, "Failed to stop this component");
   }
   COL_VAR(Result);
   return 0; // Component stops so we don't need to return anything
}