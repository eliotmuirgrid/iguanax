//---------------------------------------------------------------------------
// Copyright (C) 1997-2020 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LIGcomponents
//
// Description:
//
// Implementation
//
// Author: Shuo Hou
//---------------------------------------------------------------------------
#include <LIG/LIGcomponents.h>

#include <LUA/LUAutils.h>

#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

int LIGlistComponents(lua_State* L){
   COL_FUNCTION(LIGlistComponents);
   CFGmap* pConfigs = (CFGmap*) LUAgetPointerField(L, "_pConfigMap");
   COLvar Guids;
   auto Configs = pConfigs->ConfigMap;
   for (auto i = Configs.begin(); i != Configs.end(); i++) {
      const auto& Config = i.value().CoreConfig;
      Guids[Config.Name] = Config.Guid;
   }
   Guids.toLua(L);
   return 1;
}