//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LCOMrunAs
//
// Implementation
//
// Author: Matthew Sobkowski
// Date:   Monday 24 April 2023 - 3:59PM
// ---------------------------------------------------------------------------
#include <USR/USRuser.h>

#include <LCOM/LCOMrunAs.h>

#include <LUA/LUAcheck.h>
#include <LUA/LUAerror.h>
#include <LUA/LUAtable.h>
#include <LUA/LUAutils.h>

#include <COL/COLlog.h>
#include <COL/COLmap.h>
COL_LOG_MODULE;

#define LCOM_RUN_AS_REGISTRY_NAME "LCOMrunAs"

static int LCOMfetchRunAs(lua_State* L){
   COL_FUNCTION(LCOMfetchRunAs);
   lua_getfield(L, LUA_REGISTRYINDEX, LCOM_RUN_AS_REGISTRY_NAME);
   return 1;
}

int LCOMrunAsUser(lua_State* L){
   COL_FUNCTION(LCOMrunAsUser);
   if (0 == lua_gettop(L)) {  // component.runAsUser()
      COL_TRC("Fetch current run as and return");
      return LCOMfetchRunAs(L);
   }
   LUAcheckArgCount(L, 1, -1);
   LUAparamIsTable(L, 1);
   COLstring User = LUAexpectedStringParam(L, 1, "user");
   COL_TRC("Checking user " << User);
   COLmap<COLstring, USRuser>* pUserMap = (COLmap<COLstring, USRuser>*) LUAgetPointerField(L, "LUAuserMap");
   if (pUserMap->count(User) == 0) {
      return LUAraiseLuaError(L, "Invalid user '" + User + "'.");
   }
   COL_TRC("Setting run as to " << User);
   LUAsetStringField(L, LCOM_RUN_AS_REGISTRY_NAME, User);
   return 0;
}

bool LCOMgetRunAs(lua_State* L, COLstring* pUser) {
   COL_FUNCTION(LCOMgetRunAs);
   LCOMfetchRunAs(L);
   if (lua_isnil(L, -1)) {
      return false;
   }
   *pUser = LUAtoString(L, -1);
   return true;
}
