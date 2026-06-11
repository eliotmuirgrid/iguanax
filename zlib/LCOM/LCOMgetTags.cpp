// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LCOMgetTags
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Monday 22 April 2024 - 01:09PM
// ---------------------------------------------------------------------------

#include <LCOM/LCOMgetTags.h>

#include <LUA/LUAerror.h>
#include <LUA/LUAopen.h>
#include <LUA/LUAutils.h>

#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

int LCOMgetTags(lua_State* L){
   COL_FUNCTION(LCOMgetTags);
   const COLstring Component = LUAcomponentId(L);
   CFGmap* pNodeConfigs = (CFGmap*) LUAgetPointerField(L, "_pConfigMap");
   CFGconfig* pConfig = CFGmapNode(pNodeConfigs, Component);
   if (pConfig){
      COL_TRC("This component has " << pConfig->CoreConfig.Tags.size() << " tags");
      COLvar Tags;
      Tags.setArrayType();
      for (const auto & Tag : pConfig->CoreConfig.Tags) { Tags.push_back(Tag); }
      Tags.toLua(L);
   } else {
      // Should never happen
      LUAraiseLuaError(L, "Could not find the tags for this component");
   }
   return 1;
}