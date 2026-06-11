//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUArun
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Wednesday Oct 5 2022
//---------------------------------------------------------------------------
#include <BLUA/BLUArun.h>

#include <BLUA/BLUAcore.h>
#include <BLUA/BLUAinstance.h>
#include <BLUA/BLUAtask.h>
#include <BLUA/BLUAutils.h>
#include <CFG/CFGconfig.h>
#include <CFG/CFGconfigApi.h>
#include <COL/COLlog.h>
#include <DEP/DEPcontains.h>
#include <DEP/DEPmessaging.h>
#include <DEP/DEPlist.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <LENV/LENVcache.h>
#include <LNET/LNETcurl.h>
#include <LOG/LOGlog.h>
#include <LUA/LUAopen.h>
#include <LUA/LUAtimeout.h>
#include <LUA/LUAutils.h>
#include <SCK/SCKloop.h>
#include <SDB/SDBcapi.h>

COL_LOG_MODULE;


// Checks to see if the BLUAinstance has a force kill set, if so longjumps back to the job queue's
// take to grab the next idle and kill tasks to stop the component.
static void BLUAinstanceCheckKill(lua_State* L) {
   // COL_FUNCTION(BLUAinstanceCheckKill); // called very often
   BLUAinstance2* pInstance = BLUAinstanceGet(L);
   if (pInstance->m_ForceKill){
      COL_TRC("Kill has been set, error out of Lua execution, but specially so stop on error won't let it continue running");
      int BLUAthreadId = BLUAprintThreadId(L);
      COL_DBG("Kill set on thread " << BLUAthreadId);
      lua_settop(L, 0);
      lua_pushinteger(L, BLUA_FORCE_KILLED);
      luaL_error(L, "Component was force killed"); // never returns; unwinds through Lua/C correctly
   }
   COL_DBG("No kill requested, continue execution"); // called often, use debug to see this line
   return;
}

static void BLUAcompileDebugHook(lua_State* L, lua_Debug* pD) {
   // COL_FUNCTION(BLUAcompileDebugHook);  // Noisy.
   BLUAinstanceCheckKill(L);
   LUAcheckCompileTimeout(L);
   LUAcheckTimeout(L);
}

static void BLUAremoveCompileDebugHook(lua_State* L){
   COL_FUNCTION(BLUAremoveCompileDebugHook);
   luaopen_debug(L);
   lua_sethook(L, &BLUAcompileDebugHook, 0, 0);
   lua_pop(L, 1);
}

// For compile time
static void BLUAsetupCompileHooks(lua_State* L){
   COL_FUNCTION(BLUAsetupCompileHooks);
   luaopen_debug(L);
   lua_sethook(L, &BLUAcompileDebugHook, LUA_MASKCOUNT, 80);
   lua_pop(L, 1);
}

static void BLUAinstanceCheckTimeoutAndKill(lua_State* L, lua_Debug* pD) {
   // COL_FUNCTION(BLUAinstanceCheckTimeoutAndKill);  // Noisy.
   // COLASSERT(pD->event == LUA_HOOKCOUNT);
   BLUAinstanceCheckKill(L);
   LUAcheckTimeout(L);
}

// For run time
static void BLUAsetupInstructionHooks(lua_State* L){
   COL_FUNCTION(BLUAsetupInstructionHooks);
   luaopen_debug(L);
   lua_sethook(L, &BLUAinstanceCheckTimeoutAndKill, LUA_MASKCOUNT, 80); // We can only have 1 hook of type maskcount so we have to combine the timeout and kill checks
   lua_pop(L, 1);
}

BLUAtask BLUArunTask() {
   BLUAtask Task;
   Task.Id = BLUA_RUN;
   return Task;
}

static void BLUAgitStartupMessage(const COLstring& ComponentId, bool IsDev, const COLstring& Commit, const COLstring& Dir){
   COL_FUNCTION(BLUAgitStartupMessage);
   int LogType = SDB_INFO;
   COLstring Log = "Compiling Lua scripts.\n";
   Log += "Using Commit: ";
   if (IsDev){
      LogType = SDB_WARNING;
      Log += " NOT VERSIONED (development mode - not suitable for production)\n";
   } else {
      Log += Commit + "\n";
   }
   Log += "Dir: " + Dir + "\n";
   LOGlog(LOGlogInfo(SDB_DEBUG, ComponentId, "", "", 0, 0), Log);
}

// For the main worker, snapshot environment variable values. Clones will inherit these values
static void BLUAsetupEnvironmentCache(lua_State* L, const COLvar& Payload, BLUAinstance2* pInstance){
   COL_FUNCTION(BLUAsetupEnvironmentCache);
   if (!Payload.isMap() || !Payload.exists("worker")) { return; }// TODO error case
   COLvar Environment;
   if (Payload["worker"].asInt() == 0) {
      Environment = LENVgetEnvironment();
      pInstance->m_Fields = Environment; // store the environment, clones will use this to populate their cache
   } else {
      Environment = pInstance->m_Fields;
   }
   LENVcache(L, Environment);
}

bool BLUAcontainsDeprecated(const COLstring Dir, const COLstring& ComponentId){
   COL_FUNCTION(BLUAcontainsDeprecated);
   bool Found = false;
   COLarray<COLstring> Lines;
   DEPreadLua(&Lines);
   for(int i=0; i < Lines.size(); i++){
      COLstring Line = Lines[i];
      if(DEPdirContainsFunc(Dir, Line)){
         COL_TRC("Deprecated function!" << Line);
         LOGlog(LOGlogInfo(SDB_WARNING, ComponentId, "", "", 0, 0), DEPmessage(Line));
         Found = true;
      }
   }
   return Found;
}

// LUA THREAD
void BLUAhandleRun(lua_State*& L, const COLvar& Payload, int* pResult, BLUAinstance2* pInstance) {
   COL_FUNCTION(BLUAhandleRun);
   COLASSERT(L == NULL);
   const CFGcore&   Configs     = pInstance->m_Config.CoreConfig;
   const COLstring& ComponentId = Configs.Guid;
   const bool       IsDev       = CFGdevelopmentMode(Configs.Commit, false);
   const COLstring& MainDir = DIRcomponentProjectDir(ComponentId, IsDev);
   L = LUAopen(LUA_PRODUCTION_MODE);
   LUAsetIdAndRoot(L, ComponentId, MainDir);
   BLUAsetLuaRequirePath(L, MainDir);
   BLUAaddLuacomLoader(L);
   BLUAinstanceRegister(L, pInstance);
   BLUAsetupEnvironmentCache(L, Payload, pInstance);
   BLUAgitStartupMessage(ComponentId, IsDev, Configs.Commit, MainDir);
   COLstring Main = MainDir + COL_T("main.lua");
   bool MainExists = FILfileExists(Main);
   COL_VAR2(Main, MainExists);
   if (!MainExists || luaL_loadfile(L, Main.c_str())) {
      if (!MainExists) {
         COLstring Error = "main.lua not found in " + FILpathDir(Main);
         lua_pushstring(L, Error.c_str());
      }
      COL_ERR("Failed to compile lua script");
      *pResult = BLUA_ERROR_COMPILE;   // special error code to distinguish from normal runtime error
      return;
   }
   if(BLUAcontainsDeprecated(MainDir, ComponentId)){
      pInstance->m_Config.Status.HtmlTooltipBlock = DEPtooltipHtml(ComponentId);
   };
   LUAbeginScript(L); // We need the runtime timeouts available for db connect, ftp, etc.
   LUAsetCompileTimeout(L); // store the current time right before compile
   BLUAsetupCompileHooks(L); // set up both kill check and compile timeout check
   COL_TRC("Starting compile");
   *pResult = lua_pcall(L, 0, 0, 0);
   // script is compiled, remove compile hook and set runtime hook
   COL_TRC("Compile done");
   BLUAremoveCompileDebugHook(L);
   LUAbeginScript(L);
   BLUAsetupInstructionHooks(L);
   COL_CRASH_HERE_IF_ENV_VAR_IS_SET();
}
