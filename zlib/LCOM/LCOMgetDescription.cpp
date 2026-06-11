// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LCOMgetDescription
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Wednesday 25 October 2023 - 02:51PM
// ---------------------------------------------------------------------------

#include <LCOM/LCOMgetDescription.h>

#include <LUA/LUAerror.h>
#include <LUA/LUAopen.h>
#include <LUA/LUAutils.h>

#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

int LCOMgetDescription(lua_State* L){
   COL_FUNCTION(LCOMgetDescription);
   const COLstring Component = LUAcomponentId(L);
   CFGmap* pNodeConfigs = (CFGmap*) LUAgetPointerField(L, "_pConfigMap");
   CFGconfig* pConfig = CFGmapNode(pNodeConfigs, Component);
   if (pConfig){
      COL_VAR(pConfig->CoreConfig.Description);
      LUApushString(L, pConfig->CoreConfig.Description);
   } else {
      // Should never happen
      LUAraiseLuaError(L, "Could not find the description of this component");
   }
   return 1;
}