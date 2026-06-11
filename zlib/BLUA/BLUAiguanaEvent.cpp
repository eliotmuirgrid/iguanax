#include <BLUA/BLUAiguanaEvent.h>
#include <BLUA/BLUAmanager.h>
#include <BLUA/BLUAsafe.h>
#include <BLUA/BLUAutils.h>
#include <CFG/CFGconfigApi.h>
#include <CFG/CFGmap.h>
#include <COL/COLclosure.h>
#include <COL/COLlog.h>
#include <COL/COLsplit.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <LENV/LENVcache.h>
#include <LUA/LUAopen.h>
#include <LUA/LUAstartup.h>
#include <LUA/LUAtimeout.h>
#include <LUA/LUAutils.h>
#include <SCK/SCKloop.h>
#include <SDB/SDBcapi.h>
COL_LOG_MODULE;

struct BLUAiguanaEventTracker {
   BLUAiguanaEventTracker(BLUAiguanaEventType type, int remaining, SCKloop* loop, COLclosure0* done)
       : Type(type), Remaining(remaining), pLoop(loop), pDone(done) {}
   BLUAiguanaEventType Type      = STARTUP;
   int                 Remaining = 0;
   SCKloop*            pLoop     = nullptr;
   COLclosure0*        pDone     = nullptr;
};

static void BLUAiguanaEventComplete(BLUAiguanaEventTracker* pTracker, COLvar* pResult) {
   COL_FUNCTION(BLUAiguanaEventComplete);
   const COLvar& Result = *pResult;
   COL_VAR(Result.json(1));
   pTracker->Remaining--;
   const COLstring FunctionName = pTracker->Type == STARTUP ? "Startup" : "Shutdown";
   if(!Result["error"].isNull()) {
      COL_TRC(Result["component"].asString() + " had an error");
      SDBIlogComponent(Result["error"].asString() + "\n" + FunctionName + " function was not run.", Result["component"], "#error");
   }
   delete pResult;
   if(pTracker->Remaining == 0) {
      COL_TRC("Done running all");
      pTracker->pDone->runAndDelete();
      delete pTracker;
   }
}

static bool BLUAiguanaEventCheck(lua_State* L, COLvar* pResult) {
   COL_FUNCTION(BLUAiguanaEventCheck);
   if(lua_isnil(L, -1)) {
      COL_TRC("Iguana event function in index is nil");
      return false;
   }
   if(!lua_isstring(L, -1)) {
      COL_TRC("Iguana event function is not a string");
      (*pResult)["error"] = "Iguana event function not found";
      return false;
   }
   return true;
}

static void BLUAiguanaEventGetType(lua_State* L, const BLUAiguanaEventType& Type) {
   COL_FUNCTION(BLUAiguanaEventGetType);
   switch(Type) {
      case STARTUP: lua_getfield(L, LUA_REGISTRYINDEX, "LIGstartupFunction"); break;
      case SHUTDOWN: lua_getfield(L, LUA_REGISTRYINDEX, "LIGshutdownFunction"); break;
      default: lua_pushnil(L); break;
   }
}

static void BLUAiguanaEventCheckTimeout(lua_State* L, lua_Debug* pD) {
   // COL_FUNCTION(BLUAiguanaEventCheckTimeout);  // Noisy.
   LUAcheckCompileTimeout(L);
}

static void BLUAiguanaEventSetupTimeoutCheck(lua_State* L){
   luaopen_debug(L);
   lua_sethook(L, &BLUAiguanaEventCheckTimeout, LUA_MASKCOUNT, 80);
   lua_pop(L, 1);
}

static void BLUAiguanaEventRemoveCompileDebugHook(lua_State* L){
   COL_FUNCTION(BLUAiguanaEventRemoveCompileDebugHook);
   luaopen_debug(L);
   lua_sethook(L, &BLUAiguanaEventCheckTimeout, 0, 0);
   lua_pop(L, 1);
}

static void BLUAiguanaEventLua(const COLstring& Component, const COLstring& MainDir, const BLUAiguanaEventType& Type,
                               COLvar* pResult) {
   COL_FUNCTION(BLUAiguanaEventLua);
   const COLstring MainLua = FILpathAppend(MainDir, "main.lua");
   if(!FILfileExists(MainLua)) {
      COL_TRC(MainLua << " not found");
      return;
   }
   COL_TRC("Setting up lua environment");
   lua_State* L = LUAopen(LUA_PRODUCTION_MODE);
   LUALmarkEnvironmentAsStartup(L); // Turn a lot of our apis into no-ops during startup mode.
   LUAsetIdAndRoot(L, Component, MainDir);
   BLUAsetLuaRequirePath(L, MainDir);
   BLUAaddLuacomLoader(L);
   LENVcache(L, LENVgetEnvironment());
   LUAsetCompileTimeout(L); // store the current time right before compile
   BLUAiguanaEventSetupTimeoutCheck(L); // set up both kill check and timeout check
   COL_TRC("Loading " << MainLua);
   if(luaL_loadfile(L, MainLua.c_str())) {
      COL_ERR("Failed to compile lua script");
      (*pResult)["error"] = "Failed to compile script: " + COLstring(lua_tostring(L, -1));
   } else {
      lua_pcall(L, 0, 0, 0);  // execute script to get custom global function
      BLUAiguanaEventRemoveCompileDebugHook(L);
      BLUAiguanaEventGetType(L, Type);
      BLUAiguanaEventCheck(L, pResult);
      const COLstring FunctionName = LUAtoString(L, -1);
      lua_getglobal(L, FunctionName.c_str());
      if(lua_isfunction(L, -1)) {
         COL_TRC(Component + ": Found " << FunctionName << " -- executing");
         lua_pcall(L, 0, 0, 0);
      } else {
         COL_TRC(FunctionName << " not found");
      }
   }
   lua_close(L);
   L = NULL;
}

static void BLUAiguanaEventHandle(COLstring Component, COLstring Commit, BLUAiguanaEventTracker* pTracker) {
   COL_FUNCTION(BLUAiguanaEventHandle);
   auto pResult             = new COLvar();
   (*pResult)["component"]  = Component;
   const bool       IsDev   = CFGdevelopmentMode(Commit, false);
   const COLstring& MainDir = DIRcomponentProjectDir(Component, IsDev);
   BLUAiguanaEventLua(Component, MainDir, pTracker->Type, pResult);
   pTracker->pLoop->post(COLnewClosure0(&BLUAiguanaEventComplete, pTracker, pResult));
}

void BLUAiguanaEvent(COLclosure0* pDone, SCKloop* pLoop, CFGmap* pMap, BLUAiguanaEventType Type) {
   COL_FUNCTION(BLUAiguanaEvent);
   COLstring State = STARTUP == Type ? "startup" : "shutdown";
   COLstring Message = "Running Iguana " + State + " scripts...";
   COLstring Tag = "#" + State;
   SDBIlogService(Message, Tag);
   if(BLUAsafeMode()) {
      SDBIlogService("Iguana events are disabled in safe mode");
      COLcout << "Iguana events are disabled in safe mode." << newline;
      pDone->runAndDelete();
      return;
   }
   if (pMap->ConfigMap.size() == 0) {
      COL_TRC("Early return - no components present.");
      pDone->runAndDelete();
      return;
   }
   auto* pTracker = new BLUAiguanaEventTracker(Type, pMap->ConfigMap.size(), pLoop, pDone);

   for(const auto& it : pMap->ConfigMap) {
      COL_VAR(it.first);
      pLoop->threadPool()->scheduleTask(
          COLnewClosure0(&BLUAiguanaEventHandle, it.second.CoreConfig.Guid, it.second.CoreConfig.Commit, pTracker));
   }
}
