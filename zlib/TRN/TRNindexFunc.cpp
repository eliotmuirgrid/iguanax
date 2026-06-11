//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNindexFunc
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thursday, November 18th, 2010 @ 01:50:05 PM
//---------------------------------------------------------------------------
#include <TRN/TRNindexFunc.h>
#include <TRN/TRNtrace.h>
#include <TRN/TRNfile.h>
#include <TRN/TRNfunction.h>
#include <TRN/TRNcfunction.h>
#include <TRN/TRNannotationLuaFunction.h>
#include <TRN/TRNannotationCFunction.h>

#include <LUA/LUAutils.h>

#include <LNET/LNETtcp.h>
#include <LNET/LNETcurlFtp.h>
#include <NODL/NODLconstants.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

bool TRNisLoadTable(const COLstring& Name){
   if (Name == "package"){ return true; }
   if (Name == "_G")     { return true; }
   return false;
}

void TRNreplaceCfunction(lua_State* L, int i, TRNcfunction* pCFunction){
   COL_FUNCTION(TRNreplaceCfunction);
   COL_DBG("Before closure:" << LUAstackAsString(L));
   lua_pushlightuserdata(L, pCFunction);
   lua_pushcclosure(L, &TRNtraceC, 2);
   COL_DBG("After closure:" << LUAstackAsString(L));
   lua_pushvalue(L, -2);  // grab the key again
   lua_insert(L, lua_gettop(L)-2); // push it under the closure
   COL_DBG("About to call rawset:" << LUAstackAsString(L));
   lua_rawset(L, i);
   COL_DBG("After rawset:" << LUAstackAsString(L));
}

static void TRNindexSpecificGlobal(lua_State* L, COLhashmap<COLstring, TRNcfunction*>& CFunctionList, const char* pGlobalName, TRNcfunction::eFunctionId FunctionId){
   COL_FUNCTION(TRNindexSpecificGlobal);
   lua_pushstring(L, pGlobalName);
   lua_getglobal(L, pGlobalName);
   TRNcfunction* pCfunction = TRNaddCFunction(CFunctionList, COLstring("_G.") + pGlobalName);
   pCfunction->m_FunctionId = FunctionId;
   TRNreplaceCfunction(L, LUA_GLOBALSINDEX, pCfunction);
}

static void TRNindexSpecificGlobals(lua_State* L, COLhashmap<COLstring, TRNcfunction*>& CFunctionList){
   COL_FUNCTION(TRNindexSpecificGlobals);
   TRNindexSpecificGlobal(L, CFunctionList, "pcall", TRNcfunction::PCALL);
   TRNindexSpecificGlobal(L, CFunctionList, "error", TRNcfunction::ERROR);
}

static bool TRNisMetaMethod(lua_State* L, int i){
   COLPRECONDITION(lua_isstring(L, i));
   size_t Length = 0;
   const char* pFunctionName = lua_tolstring(L, i, &Length);
   return Length > 2 && pFunctionName[0] == '_' && pFunctionName[1] == '_';
}

static void TRNscanTable(lua_State* L, int i, COLhashmap<const void*,int,COLhashPointer<const void*> >& SavedTables, COLhashmap<COLstring, TRNcfunction*>& CFunctionList, 
                  COLhashmap<const void*, COLstring, COLhashPointer<const void*> >& LFunctionNameMap, COLstring LibraryNameSoFar){
   COL_FUNCTION(TRNscanTable);
   int Top = lua_gettop(L);
   COL_VAR2(i, Top);
   lua_pushnil(L);  /* first key */
   while (0 != lua_next(L, i) != 0) {
      // uses 'key' (at index -2) and 'value' (at index -1)
      if (lua_type(L, -1) == LUA_TFUNCTION && lua_type(L, -2) == LUA_TSTRING){
         if (lua_iscfunction(L, -1) ){
            COL_VAR2(lua_tocfunction(L, -1), LUAtoString(L,-2));
            lua_CFunction pCFunction = lua_tocfunction(L,-1);
            if (pCFunction != &TRNtraceC && !TRNisMetaMethod(L, -2)){
               const COLstring FunctionName = LibraryNameSoFar.is_null() ?  LUAtoString(L, -2) : LibraryNameSoFar + "." + LUAtoString(L, -2);
               TRNcfunction* pCfunction = TRNaddCFunction(CFunctionList, FunctionName);
               TRNreplaceCfunction(L, i, pCfunction);
            } else {
               lua_pop(L, 1);
            }
         } else {
            // We cache global function names to increase the chances that annotations
            // will show the correct names (they could be called anonymously).
            TRNsetLuaFunctionName(LFunctionNameMap,lua_topointer(L, -1), LUAtoString(L, -2));
            lua_pop(L, 1);
         }
      } else if (lua_type(L, -1) == LUA_TTABLE && lua_type(L, -2) == LUA_TSTRING){
         COLstring SubTableName = LUAtoString(L,-2);
         const COLstring TempLibraryNameSoFar = LibraryNameSoFar;
         LibraryNameSoFar = LibraryNameSoFar.is_null() ? SubTableName : LibraryNameSoFar + "." + SubTableName;
         COL_TRC("Scanning sub table: " << SubTableName);
         if (!TRNisLoadTable(SubTableName)){
            COL_VAR(SubTableName);
            const void* pThisTable = lua_topointer(L,-1);
            if (SavedTables.find(pThisTable) == 0){
               SavedTables.add(pThisTable,1);
               TRNscanTable(L, lua_gettop(L), SavedTables, CFunctionList, LFunctionNameMap, LibraryNameSoFar);
               LibraryNameSoFar = TempLibraryNameSoFar;
            }
            else {
               COL_TRC("Ignore " << SubTableName << " because already scanned.");
            }
         } else {
            COL_TRC("Ignore " << SubTableName);
         }
         lua_pop(L, 1);
      } else {
         COL_TRC("Not a function.  So pop value." << LUAstackAsString(L));
         lua_pop(L, 1);
         COL_TRC("After popping value." << LUAstackAsString(L));
      }
   }
   COL_TRC("Pop key" << LUAstackAsString(L));
   COLPRECONDITION(Top == lua_gettop(L));
}

static void TRNindexMetatableFunctions(lua_State* L, COLhashmap<COLstring, TRNcfunction*>& CFunctionList, 
                                       COLhashmap<const void*, COLstring, COLhashPointer<const void*> >& LFunctionNameMap, const char* pMetatableName){
   COL_FUNCTION(TRNindexMetatableFunctions);
   COLhashmap<const void*,int,COLhashPointer<const void*> > SavedTables;

   lua_getfield(L, LUA_REGISTRYINDEX, pMetatableName); // push metatable
   if (lua_isnil(L, -1)){
      lua_pop(L, 1); // pop metatable
   } else {
      lua_getfield(L, -1, "__index"); // push __index
      TRNscanTable(L, lua_gettop(L), SavedTables, CFunctionList, LFunctionNameMap, COLstring());
      lua_pop(L, 2); // pop __index and metatable
   }
}

void TRNindexFuncGlobal(lua_State* L, COLhashmap<COLstring, TRNcfunction*>& CFunctionList, 
                        COLhashmap<const void*, COLstring, COLhashPointer<const void*> >& LFunctionNameMap){
   COL_FUNCTION(TRNindexFuncGlobal);
   TRNindexSpecificGlobals(L, CFunctionList);
   
   COLhashmap<const void*,int,COLhashPointer<const void*> > SavedTables;
   TRNscanTable(L, LUA_GLOBALSINDEX, SavedTables, CFunctionList, LFunctionNameMap, COLstring());
   // Index known metatable functions.
   TRNindexMetatableFunctions(L, CFunctionList, LFunctionNameMap, LUA_FILEHANDLE);  // ELIOT - the below constants were in NODL, LNET etc. bringing them into to here brought in a heap of dependencies.
   TRNindexMetatableFunctions(L, CFunctionList, LFunctionNameMap, "tcp_socket");
   TRNindexMetatableFunctions(L, CFunctionList, LFunctionNameMap, "ftp");
   TRNindexMetatableFunctions(L, CFunctionList, LFunctionNameMap, "CurlFtpMeta_sftp");
   TRNindexMetatableFunctions(L, CFunctionList, LFunctionNameMap, "CurlFtpMeta_sftps");
   TRNindexMetatableFunctions(L, CFunctionList, LFunctionNameMap, "dbconnection");
   // TRNindexMetatableFunctions(L, Program, NODL_DBS_METATABLE);
}
