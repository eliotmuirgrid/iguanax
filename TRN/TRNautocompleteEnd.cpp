//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: TRNautocompleteEnd.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   13/12/23 3:24 PM
//  ---------------------------------------------------------------------------
#include <TRN/TRNautocompleteEnd.h>
#include <TRN/TRNannotationState.h>
#include <TRN/TRNcompile.h>
#include <TRN/TRNrequire.h>

#include <LUA/LUAopen.h>
#include <LUA/LUAtest.h>
#include <LUA/LUAutils.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static bool TRNautoCompleteEndRecompile(const COLstring& Code, TRNannotationState* pState) {
   COL_FUNCTION(TRNautoCompleteEndRecompile);
   // This setup is needed to make sure all our functions get registered in this temporary state
   // especially the require override. Otherwise, if a require or one of our functions is called
   // in global scope of the Lua script we have a compile error
   lua_State *L = LUAopen(LUA_ANNOTATION_MODE);
   LUAsetIdAndRoot(L, pState->m_TranslatorGuid, pState->m_ProjectPath);
   LUALmarkEnvironmentAsTest(L);
   TRNannotationStateSet(L, pState);
   TRNrequireOverride(L);
   const int CompileResult = luaL_dostring(L, Code.c_str());
   COLstring CompileError = LUAtoString(L, -1);
   lua_close(L);
   COL_VAR(CompileError);
   COL_VAR(CompileResult);
   return CompileResult == 0;
}

static COLstring TRNautoCompleteEndInsert(const COLstring& Code, const int Line) {
   COL_FUNCTION(TRNautoCompleteEndInsert);
   COLstring NewCode = Code;
   NewCode.replace("\r\n", "\n");
   size_t NewlineCount = 0;
   size_t Pos = 0;
   while (NewlineCount <= Line && Pos != npos) {
      Pos = NewCode.find('\n', Pos + 1);
      NewlineCount++;
   }
   if (Pos == npos) { NewCode += "\nend"; } // at end of file so just append
   else             { NewCode.insert(Pos, "\nend"); }

   return NewCode;
}


void TRNautocompleteEnd(const COLstring& Code, int Line, COLvar* pResult, TRNannotationState* pState) {
   COL_FUNCTION(TRNautocompleteEnd);
   COL_VAR(Code);
   COL_VAR2(Line, pResult->json(1));
   const COLvar ErrorVar = (*pResult)["ScriptError"];
   if(ErrorVar["Description"].asString().find("'end' expected ") == npos) {
      COL_TRC("Not an 'end' error");
      return;
   }
   const COLstring NewCode = TRNautoCompleteEndInsert(Code, Line);
   COL_VAR(NewCode);
   if(TRNautoCompleteEndRecompile(NewCode, pState)) {
      COL_TRC("Compilation suceeded");
      (*pResult)["code"] = NewCode;
      (*pResult)["autocomplete"] = true;
   } else {
      COL_TRC("Compilation failed");
      (*pResult)["autocomplete"] = false;
   }
   COL_VAR(pResult->json(1));
}