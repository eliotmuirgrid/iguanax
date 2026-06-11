// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LCOMclone
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
 
// Date:   Monday 25 November 2024 - 10:48AM
// ---------------------------------------------------------------------------

#include <LCOM/LCOMclone.h>

#include <BLUA/BLUAcore.h>
#include <BLUA/BLUAinstance.h>
#include <CFG/CFGconfig.h>

#include <LUA/LUAerror.h>
#include <LUA/LUAtable.h>
#include <LUA/LUAstartup.h>
#include <LUA/LUAtest.h>
#include <LUA/LUAutils.h>

#include <SCK/SCKloop.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

#define LCOM_COMPONENT_CLONE_META "LCOM_COMPONENT_CLONE_METATABLE"
#define LCOM_COMPONENT_CLONE "LCOM_COMPONENT_CLONE"

// This happens on Main thread
static void LCOMsetupLuaPool(int Count, BLUAinstanceManager* pManager, BLUAinstance2* pInstance, SCKloop* pLoop){
   COL_FUNCTION(LCOMsetupLuaPool);
   COL_TRC("Will create " << Count << " instances");
   pInstance->m_CloneCount = Count;
   // the lua instance that started this call stays in use so only spawn Count - 1 clones
   BLUAinstanceSpawn(pLoop, pManager, pInstance, Count - 1);
   COL_TRC("All instances spawned.");
}  

// component.spawnClone{count=n}
int LCOMspawnClone(lua_State* L){
   COL_FUNCTION(LCOMspawnClone);
   int Count = LUAexpectedIntParam(L, 1, "count");
   if (Count < 2 || Count > 100) { 
      COL_TRC("Since we spin up count - 1 new clones, count should be 2 or more. Also setting a max of 100 just in case.");
      return luaL_error(L, "Count must be between 2 and 100");
   }
   if (LUALisEnvironmentTest(L) || LUALisEnvironmentStartup(L)) {
      COL_TRC("This is IDE mode - no-op");
      return 0;
   }
   BLUAinstance2* pInstance = BLUAinstanceGet(L);
   if (pInstance->m_CloneCount != 0){
      COL_TRC("Component already cloned - no-op");
      return 0;
   }
   if (pInstance->m_Config.CoreConfig.Sources.size() != 0){
      COL_TRC("This is probably ok considering how take works, but lets disable it to be safe");
      return luaL_error(L, "A component using component.spawnClone must not have upstream components.");
   }
   lua_getfield(L, LUA_REGISTRYINDEX, LCOM_COMPONENT_CLONE); // This is a closure set up when the lua state is created. It gets cleaned up by the lua garbage collector.
   COLclosure2<int, BLUAinstance2*>* pFunction = *(COLclosure2<int, BLUAinstance2*>**)lua_touserdata(L, -1);
   COL_VAR(pFunction);
   if (!pFunction) {
      COL_WRN("component.clone is not set up.");
      return luaL_error(L, "Internal Error: The API is not set up properly.");
   }
   pFunction->run(Count, pInstance);
   return 0;
}

static void LCOMcomponentCloneFromLua(int Count, BLUAinstance2* pInstance, BLUAinstanceManager* pManager, SCKloop* pLoop){
   COL_FUNCTION(LCOMcomponentCloneFromLua);
   pLoop->post(COLnewClosure0(&LCOMsetupLuaPool, Count, pManager, pInstance, pLoop));
}

// Have to do it this way to get access to pBluaManager
void LCOMsetUpClone(lua_State* L, BLUAinstanceManager* pBluaManager, SCKloop* pLoop){
   COL_FUNCTION(LCOMsetUpClone);
   COLclosure2<int, BLUAinstance2*>* pFunction = COLnewClosure2<int, BLUAinstance2*>(&LCOMcomponentCloneFromLua, pBluaManager, pLoop);
   // wrap closure in lua user data to automatically free it through garbage collection
   LUApushPointer< COLclosure2<int, BLUAinstance2*>*>(L, pFunction, LCOM_COMPONENT_CLONE_META);
   lua_setfield(L, LUA_REGISTRYINDEX, LCOM_COMPONENT_CLONE);
}
