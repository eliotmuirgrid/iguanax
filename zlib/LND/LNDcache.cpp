//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LNDcache
//
// Description:
//
// Lua node cache implementation
//
// Author: Vismay Shah
// Date: Tues Nov 22 2022
//---------------------------------------------------------------------------
#include <LND/LNDcache.h>

#include <FIL/FILutils.h>
#include <LUA/LUAutils.h>
#include <LUA/LUAerror.h>

#include <COL/COLlog.h>
#include <COL/COLdateTime.h>
COL_LOG_MODULE;

void LNDcacheFileExists(lua_State* L, const COLstring& File){
   COL_FUNCTION(LNDcacheFileExists);
   COL_VAR(File);
   COLstring   Error;
   if (!FILfileExists(File)){
      Error = "File " + File  + " does not exist.";
      COL_TRC(Error);
      LUAraiseLuaError(L, Error);
   }
}

bool LNDkeyExists(lua_State* L, const COLstring& Key){
   COL_FUNCTION(LNDkeyExists);
   lua_pushstring(L, Key.c_str());
   lua_gettable(L, LUA_REGISTRYINDEX);
   if (!lua_isnil(L, -1)) {
      lua_pop(L, 1);
      return true;
   } else {
      return false;
   }
}

void LNDgetLastWriteTime(const COLstring& File, time_t* pTime){
   COL_FUNCTION(LNDgetLastWriteTime);
   *pTime = FILmodified(File);
}

bool LNDisItemOutdated(const COLstring& File, time_t* pLastWrite, const time_t& CacheLastAccess){
   COL_FUNCTION(LNDisItemOutdated);
   LNDgetLastWriteTime(File, pLastWrite);
   COL_VAR2(CacheLastAccess, *pLastWrite);
   return (CacheLastAccess < *pLastWrite);
}

COLstring LNDvdbKey(const COLstring& File){
   COL_FUNCTION(LNDvdbKey);
   return "LNDvdb_" + File;
}

COLstring LNDvmdKey(const COLstring& File){
   COL_FUNCTION(LNDvmdKey);
   return "LNDvmd_" + File;
}