//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LIGregister.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   25/03/25 11:56 AM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <LIG/LIGregister.h>
#include <LUA/LUAerror.h>
#include <LUA/LUAtest.h>
#include <LUA/LUAutils.h>
COL_LOG_MODULE;

int LIGregister(lua_State* L) {
   COL_FUNCTION(LIGregister);
   if(LUALisEnvironmentTest(L)) {
      COL_TRC("IDE mode. no-op.");
      return 0;
   }
   if(!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
      COL_TRC("Expected string argument");
      return LUAraiseLuaError(L, "Expected string argument");
   }
   const COLstring FunctionType = LUAtoString(L, -2);
   const COLstring FunctionName = LUAtoString(L, -1);
   COL_VAR2(FunctionType, FunctionName);
   COLstring RegistryType;
   if(FunctionType == "startup") {
      RegistryType = "LIGstartupFunction";
   } else {
      RegistryType = "LIGshutdownFunction";
   }
   lua_setfield(L, LUA_REGISTRYINDEX, RegistryType.c_str());

   return 1;
}
