// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LCOMgetName
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Wednesday 25 October 2023 - 02:48PM
// ---------------------------------------------------------------------------

#include <LCOM/LCOMgetName.h>

#include <LUA/LUAerror.h>
#include <LUA/LUAopen.h>
#include <LUA/LUAutils.h>

#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

int LCOMgetName(lua_State* L){
   COL_FUNCTION(LCOMgetName);
   const COLstring Component = LUAcomponentId(L);
   CFGmap* pNodeConfigs = (CFGmap*) LUAgetPointerField(L, "_pConfigMap");
   CFGconfig* pConfig = CFGmapNode(pNodeConfigs, Component);
   if (pConfig){
      COL_VAR(pConfig->CoreConfig.Name);
      LUApushString(L, pConfig->CoreConfig.Name);
   } else {
      // Should never happen
      LUAraiseLuaError(L, "Could not find the name of this component");
   }
   return 1;
}