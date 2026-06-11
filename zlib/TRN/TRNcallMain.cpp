//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNcallMain
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday, November 17th, 2010 @ 03:27:57 PM
//---------------------------------------------------------------------------
#include <TRN/TRNcallMain.h>
#include <TRN/TRNextractErrorString.h>

#include <LUA/LUAtimeout.h>
#include <LND/LNDdebug.h>

#include <LUA/LUAutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

bool TRNcallFunctionWithSampleData(lua_State* L, const COLstring& SampleFunction, const COLstring& SampleData, COLstring* pError){
   COL_FUNCTION(TRNcallFunctionWithSampleData);
   lua_settop(L, 1);
   COLstring Function = SampleFunction;
   if (Function.is_null()) { Function = COL_T("main"); }
   lua_getglobal(L, Function.c_str());
   if (lua_isnil(L, -1)){
      *pError = "The function " + Function + " is not defined.\n"
                "This is required as the main entry point for the script.\n"
                "Please edit and save the script to correct the error.";
      return false;
   }
   COL_VAR(LUAstackAsStringCustom(L, &LNDuserDataFormatter));
   LUAbeginScript(L);
   COL_VAR(LUAstackAsStringCustom(L, &LNDuserDataFormatter));
   LUApushString(L, SampleData);
   if (lua_pcall(L, 1, 0, 0)){
      COL_VAR(LUAstackAsStringCustom(L, &LNDuserDataFormatter));
      TRNextractErrorString(L);
      *pError = LUAtoString(L, -1);
      COL_VAR(*pError);
      return false;
   }
   return true;
}