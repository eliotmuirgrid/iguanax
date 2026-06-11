//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNcompile
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday, November 17th, 2010 @ 12:55:34 PM
//---------------------------------------------------------------------------
#include <TRN/TRNcompile.h>
#include <TRN/TRNannotationState.h>
#include <TRN/TRNconstant.h>
#include <TRN/TRNextractErrorString.h>
#include <TRN/TRNfileFind.h>
#include <TRN/TRNfile.h>
#include <TRN/TRNloop.h>
#include <TRN/TRNrequire.h>

#include <LUA/LUAutils.h>

#ifdef _WIN32
#include <LUACOM/LUACOMcreateDestroy.h>
#endif
#include <LENV/LENVcache.h>
#include <LHLP//LHLPindex.h>
#include <LHLP/LHLPmapMethods.h>

#include <LUA/LUAtimeout.h>

#include <LND/LNDdebug.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

#define TRN_TIMEOUT_CHECK_FREQUENCY 80

static void TRNcompileCheckTimeout(lua_State* L, lua_Debug* pD){
   COL_FUNCTION(TRNcompileCheckTimeout);
   COLPRECONDITION(pD->event == LUA_HOOKCOUNT);
   LUAcheckCompileTimeout(L);
   LUAcheckTimeout(L);
}

void TRNcompileHookSet(lua_State* L){
   COL_FUNCTION(TRNcompileHookSet);
   lua_sethook(L, &TRNcompileCheckTimeout, LUA_MASKCOUNT, TRN_TIMEOUT_CHECK_FREQUENCY);
   lua_pop(L, 1);
   LUAsetCompileTimeout(L);
   LUAbeginScript(L);
}

void TRNcompileHookClear(lua_State* L){
   COL_FUNCTION(TRNcompileHookClear);
   lua_sethook(L, &TRNcompileCheckTimeout, 0, 0);
}

void TRNdestroyLua(lua_State** pL){
   COL_FUNCTION(TRNdestroyLua);
   if (*pL){
      COL_TRC("Destroy lua interpreter.");
      lua_close(*pL);
      *pL = 0;
   }
}

void TRNcompileRedoWithLoopEnd(lua_State* L, const COLstring& Source, const COLstring& FileName){
   COL_FUNCTION(TRNcompileRedoWithLoopEnd);
   COLstring ModifiedCode = Source;
   if (!TRNloopInsertEndCalls(&ModifiedCode)){
      COL_TRC("No loops found so first compile is just fine.");
      COL_VAR(ModifiedCode.equals(Source));
      return;
   }
   if (luaL_loadbuffer(L, ModifiedCode.c_str(), ModifiedCode.size(), FileName.c_str())){
      // Error - pop the error and leave unmodified code on the stack
      lua_pop(L, 1);
      // TODO something more to "report" this error so it doesn't go unnoticed?
      COL_ERR("Modified version of " << FileName << " had a syntax error, while the unmodified version did not.");
   } else {
      COL_TRC("Replaced compiled code with modified chunk.");
      lua_replace(L, -2); // replace unmodified chunk with modified chunk
   }   
}

bool TRNcompileDouble(lua_State* L, const COLstring& Source, const COLstring& FileName, COLstring* pError){
   COL_FUNCTION(TRNcompileDouble);
   COL_DBG("Source = \n" << Source);
   if (luaL_loadbuffer(L, Source.c_str(), Source.size(), FileName.c_str())){
      *pError = LUAtoString(L, -1);   // TODO - NWBgenerateErrorJson would do fancy stuff with parsing out the line number
      return false;
   }
   if (!TRNgetAnnotationState(L)->m_IsIntellisense){
      COL_TRC("Not an intellisense request, can add loop end calls");
      TRNcompileRedoWithLoopEnd(L, Source, FileName);
   }
   return true;
}

static void TRNenvironmentCache(lua_State* L){
   COL_FUNCTION(TRNenvironmentCache);
   COL_TRC("Cache the environment variables");
   COLvar Environment = LENVgetEnvironment();
   COL_VAR(Environment);
   LENVcache(L, Environment);
}

void TRNcompile(lua_State** pL, TRNannotationState* pState, COLstring* pError){
   COL_FUNCTION(TRNcompile);
   COL_TRC("Starting compile lua_gettop(L) = " << lua_gettop(*pL));
   luaopen_debug(*pL);
   COL_TRC("After luaopen_debug lua_gettop(L) = " << lua_gettop(*pL));
   TRNcompileHookSet(*pL); // Case #17499
   COL_TRC("After TRNcompileHookSet lua_gettop(L) = " << lua_gettop(*pL));
   TRNannotationStateSet(*pL, pState);
   COL_TRC("After TRNannotationStateSet lua_gettop(L) = " << lua_gettop(*pL));
   LNDsetNodDebug(*pL);
   COL_TRC("After LNDsetNodDebug lua_gettop(L) = " << lua_gettop(*pL));
   TRNrequireOverride(*pL);
   COL_TRC("After TRNrequireOverride lua_gettop(L) = " << lua_gettop(*pL));
   TRNenvironmentCache(*pL);
   COL_TRC("After TRNenvironmentCache lua_gettop(L) = " << lua_gettop(*pL));
   TRNfile* pMain = TRNfileFindOrAdd(pState->m_LuaFileList, pState->m_ProjectPath, TRN_MAIN_FILE_NAME);
   COL_TRC("After TRNfileFindOrAdd lua_gettop(L) = " << lua_gettop(*pL));
   if (!TRNcompileDouble(*pL, pMain->m_Source, TRN_MAIN_FILE_NAME, pError)){
      COL_TRC("After TRNcompileDouble failed lua_gettop(L) = " << lua_gettop(*pL));
      #ifdef _WIN32
      LUACOMdestroyLuacomWrapper(*pL, pState->m_UsesLuacom);
      #endif
      TRNdestroyLua(pL);
      return;
   }
   COL_TRC("Compile");
   COL_TRC("After TRNcompileDouble lua_gettop(L) = " << lua_gettop(*pL));
   COL_VAR(LUAstackAsStringCustom(*pL, &LNDuserDataFormatter));
   COL_TRC("Before pcall lua_gettop(L) = " << lua_gettop(*pL));
   if (lua_pcall(*pL, 0, 0, 0)){
      COL_TRC("After pcall failed lua_gettop(L) = " << lua_gettop(*pL));
      TRNextractErrorString(*pL);
      *pError = LUAtoString(*pL, -1);  //  TODO - see  how we'd get the last line from the old reporter, Reporter.lastMainFileLine(), ErrorJson);
      COL_VAR(*pError);
      #ifdef _WIN32
      LUACOMdestroyLuacomWrapper(*pL, pState->m_UsesLuacom);
      #endif
      TRNdestroyLua(pL);
      return;
   }
   COL_TRC("After pcall lua_gettop(L) = " << lua_gettop(*pL));
   TRNcompileHookClear(*pL);
   COL_TRC("After TRNcompileHookClear lua_gettop(L) = " << lua_gettop(*pL));
   COL_TRC("On our way back, after calling TRNcompileHookClear");
}
