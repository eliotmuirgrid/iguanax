//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUALtimeout
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Thursday, April 7th, 2011 @ 04:22:36 PM
//
//---------------------------------------------------------------------------
#include "LUAtimeout.h"

#include <LUA/LUAerror.h>
#include <LUA/LUAopen.h>
#include <LUA/LUAutils.h>
#include <SCK/SCKloop.h>

#include <ctype.h> // isdigit
#include <stdlib.h> // atoi
#include <time.h>
#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#define LUA_COMPILE_TIMEOUT_SECONDS (60)
#define LUA_COMPILE_TIMEOUT_ENV "COMPONENT_COMPILE_TIMEOUT_SECONDS"
#define NORMAL_TIMEOUT_SECONDS (5*60)
#define NORMAL_TIMEOUT_STRING  "5 minutes"

const char* LUAL_TIMEOUT_START   = "LUAL_TIMEOUT_START";
const char* LUAL_TIMEOUT_TARGET  = "LUAL_TIMEOUT_TARGET";
const char* LUAL_TIMEOUT_STRING  = "LUAL_TIMEOUT_STRING";
const char* LUAL_TIMEOUT_CHECKER = "LUAL_TIMEOUT_CHECKER";

const char* LUAL_COMPILE_TIMEOUT_TARGET = "LUAL_COMPILE_TIMEOUT_TARGET";
const char* LUAL_COMPILE_TIMEOUT_START  = "LUAL_COMPILE_TIMEOUT_START";

void LUAbeginScript(lua_State* L){
   COL_FUNCTION(LUAbeginScript);
   int Top=lua_gettop(L), TimeoutSeconds;
   time_t Start, Target;
   const char* pTimeoutString;
   TimeoutSeconds = NORMAL_TIMEOUT_SECONDS;
   pTimeoutString = NORMAL_TIMEOUT_STRING;

   time(&Start);
   Target = Start + TimeoutSeconds;

   lua_pushinteger(L, Start);
   lua_setfield(L, LUA_REGISTRYINDEX, LUAL_TIMEOUT_START);

   // LUA_REGISTRY[LUAL_TIMEOUT_TARGET] = Target
   lua_pushinteger(L, Target);
   lua_setfield(L, LUA_REGISTRYINDEX, LUAL_TIMEOUT_TARGET);

   // LUA_REGISTRY[LUAL_TIMEOUT_STRING] = pTimeoutString
   lua_pushstring(L, pTimeoutString);
   lua_setfield(L, LUA_REGISTRYINDEX, LUAL_TIMEOUT_STRING);

   COLASSERT(Top == lua_gettop(L));
}

int LUAsetTimeout(lua_State* L){
   COL_FUNCTION(LUAsetTimeout);
   time_t Start, Now, Target;
   lua_Integer Timeout;

   if(lua_gettop(L) != 1 || !lua_isnumber(L,1)) {
      luaL_error(L, "new timeout expected, in seconds");
   }
   Timeout = lua_tointeger(L,1);
   if(Timeout < 0) Timeout = 0;

   COL_VAR(Timeout);

   lua_getfield(L, LUA_REGISTRYINDEX, LUAL_TIMEOUT_TARGET);
   if(lua_isnumber(L,-1)) {
      time(&Now);
      Target = Now + Timeout;
      lua_pushinteger(L, Target);
      lua_setfield(L, LUA_REGISTRYINDEX, LUAL_TIMEOUT_TARGET);

      lua_getfield(L, LUA_REGISTRYINDEX, LUAL_TIMEOUT_START);
      if(lua_isnumber(L,-1)) {
         Start = lua_tointeger(L,-1);
         lua_pushfstring(L, "%d seconds", (int)(Target - Start));
         lua_setfield(L, LUA_REGISTRYINDEX, LUAL_TIMEOUT_STRING);
      }
   }
   return 0;
}

void LUAcheckTimeout(lua_State* L){
   //COL_FUNCTION(LUAcheckTimeout); // too noisy
   int Top = lua_gettop(L);
   time_t Now;
   time(&Now);

   // Target = LUA_REGISTRY[LUAL_TIMEOUT_TARGET]
   lua_getfield(L, LUA_REGISTRYINDEX, LUAL_TIMEOUT_TARGET);
   time_t Target = lua_tointeger(L, -1);
   lua_pop(L, 1);

   COL_VAR2(Now,Target);
   if (Now >= Target){
      // pTimeoutString = LUA_REGISTRY[LUAL_TIMEOUT_STRING]
      lua_getfield(L, LUA_REGISTRYINDEX, LUAL_TIMEOUT_STRING);
      const char* pTimeoutString = lua_tostring(L, -1);
      lua_pop(L, 1);
      LUA_ERROR_STREAM("Timed out - script took longer than " << pTimeoutString <<  " to run.");
   }

   // Do additional timeout check, if an additional checker is supplied

   // pChecker = LUA_REGISTRY[LUAL_TIMEOUT_CHECKER]
   lua_getfield(L, LUA_REGISTRYINDEX, LUAL_TIMEOUT_CHECKER);
   if (!lua_isnil(L, -1)){
      LUAtimeoutChecker* pChecker = (LUAtimeoutChecker*)lua_touserdata(L, -1);
      pChecker->checkTimeout(L);
   }
   lua_pop(L, 1);

   COLASSERT(Top == lua_gettop(L));
}

void LUAsetTimeoutChecker(lua_State* L, LUAtimeoutChecker* pChecker){
   COL_FUNCTION(LUAsetTimeoutChecker);
   // LUA_REGISTRY[LUAL_TIMEOUT_CHECKER] = pChecker
   lua_pushlightuserdata(L, pChecker);
   lua_setfield(L, LUA_REGISTRYINDEX, LUAL_TIMEOUT_CHECKER);
}

static int LUAparseCompileTimeout(const char* pValue){
   COL_FUNCTION(LUAparseCompileTimeout);
   bool Valid = true;
   const char* p = pValue;
   for (; *p; ++p) {
      if (!isdigit((unsigned char)*p)) {
         Valid = false;
         break;
      }
   }
   if (Valid){
      int Timeout = atoi(pValue);
      if (Timeout < 5) return LUA_COMPILE_TIMEOUT_SECONDS;
      else             return Timeout;
   }
   return LUA_COMPILE_TIMEOUT_SECONDS;
}

void LUAsetCompileTimeout(lua_State* L){
   COL_FUNCTION(LUAsetCompileTimeout);
   int TimeoutSeconds = 0;
   time_t Start, Target, InfoTarget;
   TimeoutSeconds = LUA_COMPILE_TIMEOUT_SECONDS;
   // Check for environment variable and if present use that instead
   const char* pValue = getenv(LUA_COMPILE_TIMEOUT_ENV);
   if (pValue) { TimeoutSeconds = LUAparseCompileTimeout(pValue); }
   COL_VAR(TimeoutSeconds);
   time(&Start);
   Target = Start + TimeoutSeconds;
   InfoTarget = Start + TimeoutSeconds/2;
   // Set the compile start time and compile timeout target
   lua_pushinteger(L, Start);
   lua_setfield(L, LUA_REGISTRYINDEX, LUAL_COMPILE_TIMEOUT_START);
   lua_pushinteger(L, Target);
   lua_setfield(L, LUA_REGISTRYINDEX, LUAL_COMPILE_TIMEOUT_TARGET);
   COL_VAR2(Start, Target);
   COL_TRC("Script compile timeout setup done");
}

void LUAcheckCompileTimeout(lua_State* L){
   //COL_FUNCTION(LUAcheckCompileTimeout); // too noisy
   time_t Now = time(NULL);
   // Target = LUA_REGISTRY[LUAL_COMPILE_TIMEOUT_TARGET]
   lua_getfield(L, LUA_REGISTRYINDEX, LUAL_COMPILE_TIMEOUT_TARGET);
   time_t Target = lua_tointeger(L, -1);
   lua_pop(L, 1);
   // COL_VAR2(Now,Target);
   if (Now >= Target){
      LUA_ERROR_STREAM("Error: Unable to complete Lua script compilation. The script may be blocked by operations in the global scope "
                       "(e.g., database connections, network calls, or other functions that can hang). \nPlease move any potentially "
                       "blocking operations inside functions (for example, inside main() or an initialization function) so the component can start correctly.\n"
                       "The duration of the compile-time timeout can be adjusted by setting the COMPONENT_COMPILE_TIMEOUT_SECONDS environment variable");
   }
}