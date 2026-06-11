//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAinstance
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Friday May 14 2021
//---------------------------------------------------------------------------
#include "BLUAinstance.h"

#include <LUA/LUAtable.h>
#include <LUA/LUAutils.h>
#include <LUA/LUAerror.h>
#include <LUA/LUAstartup.h>
#include <LUA/LUAtest.h>
#include <LUAL/LUALutils.h>
#include <LTMR/LTMRtimer.h>
#include <SCK/SCKloop.h>
#include <CFG/CFGconfig.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


bool BLUAgetCalledFunction(const COLstring& FunctionName, lua_State* L, int* pResult){
   COL_FUNCTION(BLUAgetCalledFunction);
   COL_VAR(FunctionName);
   lua_getglobal(L, FunctionName.c_str());
   if (!lua_isfunction(L, -1)) {
      lua_pop(L, 1);
      COL_TRC("Error: " << FunctionName << " function not defined");
      *pResult = BLUA_ERROR_COMPILE; // Treating this as a compile error to make sure iguana.stopOnError can't continue execution.
      LUApushString(L, "Error calling function " + FunctionName + ". No such function defined.");
      return false;
   }
   return true;
}

void BLUAinstanceStopOnError(BLUAinstance2* pInstance, bool Value) {
   pInstance->m_StopOnError = Value;
}

static void BLUAinstanceSetStatusLight(CFGconfig* pConfig, COLstring Color) {
   COL_FUNCTION(BLUAinstanceSetStatusLight);
   COL_VAR(Color);
   if     (Color.equalsIgnoreCase("YELLOW")) pConfig->Status.Light = CFG_LIGHT::IN_PROGRESS;
   else if(Color.equalsIgnoreCase("GREEN"))  pConfig->Status.Light = CFG_LIGHT::ON;
   COL_VAR(CFGoutputLight(pConfig->Status.Light));
}

static void BLUAinstanceSetColorOnMain(BLUAinstance2* pInstance, COLstring Color) {
   COL_FUNCTION(BLUAinstanceSetColorOnMain);
   BLUAinstanceSetStatusLight(&pInstance->m_Config, Color);
}

int BLUAsetLight(lua_State* L) {
   COL_FUNCTION(BLUAsetLight);
   COLstring Color  = LUAexpectedStringParam(L, 1, "color");
   if(Color.toUpperCase() != "GREEN" && Color.toUpperCase() != "YELLOW") LUA_ERROR_STREAM("color must be 'green' or 'yellow'.");
   if (LUALisEnvironmentTest(L) || LUALisEnvironmentStartup(L)) return 0; // this is a no-op in IDE mode
   BLUAinstance2* pInstance = BLUAinstanceGet(L);
   pInstance->m_pLoop->post(COLnewClosure0(&BLUAinstanceSetColorOnMain, pInstance, Color));
   return 0;
}

void BLUAinstanceRegister(lua_State* L, BLUAinstance2* pInstance){
   COL_FUNCTION(BLUAinstanceRegister);
   LUAsetPointerField(L, "BLUAinstance", pInstance);
}

BLUAinstance2* BLUAinstanceGet(lua_State* L){
   COL_FUNCTION(BLUAinstanceGet);
   return (BLUAinstance2*)LUAgetPointerField(L, "BLUAinstance");
}

void BLUAinstanceClearScriptTimers(BLUAinstance2* pInstance){
   COL_FUNCTION(BLUAinstanceClearScriptTimers);
   COL_TRC("Cleaning up all timers for instance: " << pInstance->m_Config.CoreConfig.Guid);
   LTMRcancelAllTimers(pInstance);
}
