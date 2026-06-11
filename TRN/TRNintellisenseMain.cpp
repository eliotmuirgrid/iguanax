//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNintellisenseMain
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thursday, December 23rd, 2010 @ 10:21:13 PM
//
//---------------------------------------------------------------------------
#include <TRN/TRNintellisenseMain.h>

#include <TRN/TRNcallMain.h>
#include <TRN/TRNannotationJob.h>
#include <TRN/TRNcompile.h>
#include <TRN/TRNextractErrorString.h>
#include <TRN/TRNintellisense.h>
#include <TRN/TRNintellisenseContext.h>
#include <TRN/TRNintellisenseDebug.h>
#include <TRN/TRNparseError.h>
#include <TRN/TRNintellisenseAlterCode.h>
#include <TRN/TRNvariableAutocompletion.h>
#include <TRN/TRNfileFind.h>
#include <TRN/TRNwriteScriptError.h>


#include <LHLP/LHLPindex.h>

#include <LND/LNDdebug.h>

#include <LUA/LUAutils.h>
#include <LUA/LUAtimeout.h>
#include <LUA/LUAopen.h>
#include <LUA/LUAtest.h>

#include <LUACX/LUAXgrabOpenExpression.h>
#include <LUACX/LUAXupvalues.h>

#ifdef _WIN32
#include <LUACOM/LUACOMcreateDestroy.h>
#endif
#include <COL/COLlog.h>
COL_LOG_MODULE;

static bool TRNcodeNeedsIntellisense(TRNintellisenseContext* pContext){
   COL_FUNCTION(TRNcodeNeedsIntellisense);
   bool Result;
   Result = LUAXgrabOpenExpression(&pContext->ParseContext);
   COL_VAR(Result);
   COL_VAR3(pContext->ParseContext.FullExpression, pContext->ParseContext.TrimmedExpression, pContext->ParseContext.SearchString);
   COL_VAR2(pContext->ParseContext.IndexChar, pContext->ParseContext.LastPunctuation);
   return Result;
}

void TRNintellisenseMain(TRNintellisenseContext* pContext, COLvar* pOut, TRNannotationJob* pJob){
   COL_FUNCTION(TRNintellisenseMain);
   TRNintellisenseContext& Context = *pContext;
   COLvar& Out = *pOut;
   Out["options"].setArrayType();
   COL_VAR(Context);
   if (Context.ParseContext.FullExpression.is_null() && !TRNcodeNeedsIntellisense(&Context)){
      COL_TRC("Code doesn't need intellisense, returning.");
      return;
   }
   if (TRNshouldDoVariableAutocompletion(Context.ParseContext)){
      COL_TRC("Getting upvalues");
      LUAXgetUpvalues(Context.ParseContext.m_Code, Context.ParseContext.m_BeginLine, &Context.Upvalues);
      COL_VAR(Context.Upvalues.size());
   }

   COL_TRC("Setting up Lua state");
   lua_State* L = NULL;

   COL_TRC("Compiling code to find any script errors and to register the global help files to the Lua state");
   TRNannotationState* pState = &pJob->m_AnnotationState;
   L = LUAopen(LUA_INTELLISENSE_MODE);
   LUAsetIdAndRoot(L, pState->m_TranslatorGuid, pState->m_ProjectPath);
   COL_VAR(lua_gettop(L));
   LUALmarkEnvironmentAsTest(L);
   COL_VAR(lua_gettop(L));

   COLstring ModifiedCode(Context.ParseContext.m_Code);
   TRNintellisenseAlterCode(Context.ParseContext.m_BeginLine, Context.ParseContext.TrimmedExpression.is_null() ? Context.ParseContext.FullExpression : Context.ParseContext.TrimmedExpression, Context.ParseContext.IndexChar, &ModifiedCode);
   TRNfileFindOrAdd(pState->m_LuaFileList, pState->m_ProjectPath, Context.CurrentFile)->m_Source = ModifiedCode;
   COL_VAR(lua_gettop(L));

   COLstring Error;
   LHLPinit(&pJob->m_AnnotationState.m_HelpMap, L);
   COL_TRC("Compile");
   COL_VAR(lua_gettop(L));
   TRNcompile(&L, &pJob->m_AnnotationState, &Error);
   // Important to do this here. There is a chance the lua state was destroyed due to an error
   // ex if there's a syntax error in a required file, we destroy the state. If we then try to
   // access L and register help, there will be a segmentation fault.
   if (!Error.is_null()){
      TRNwriteScriptError(pOut, Error, pJob->m_AnnotationState, true);
      return;
   }
   COL_TRC("After TRNcompile lua_gettop" << lua_gettop(L)); // do this after error check!
   COL_TRC("Register custom help");
   LHLPregisterCustomHelp(L, &pJob->m_AnnotationState.m_HelpMap, pJob->m_AnnotationState.m_ProjectPath);
   // We have to have this after compile - we cannot put it in the compile routine since the annotation job needs to wrap all the C lua functions
   // to support annotations - but the the intellisense compile doesn't (need to) do this.
   COL_TRC("Compiling succeeded, help registered. Now setting up debug hook");
   TRNintellisenseHookSet(L);
   TRNintellisenseContextSet(L, &Context);
   Error.clear();
   COL_TRC("About to call main");
   if (!TRNcallFunctionWithSampleData(L, pJob->m_AnnotationState.m_SampleFunction, pJob->m_AnnotationState.m_SampleData, &Error) && !Context.TargetReached){
      LUArunLuaIdleCallBack(L);
      #ifdef _WIN32
      LUACOMdestroyLuacomWrapper(L, pJob->m_AnnotationState.m_UsesLuacom);
      #endif
      lua_close(L);
      TRNwriteScriptError(pOut, Error, pJob->m_AnnotationState, false);
      return;
   }
   LUArunLuaIdleCallBack(L);
   COL_VAR2(Context.Options.size(), Context.TargetReached);
   #ifdef _WIN32
   LUACOMdestroyLuacomWrapper(L, pJob->m_AnnotationState.m_UsesLuacom);
   #endif
   lua_close(L);
   COL_TRC("Execution done, preparing web response.");
   TRNintellisenseJson(pOut, Context, Context.LineNumber, Context.ParseContext.m_Cursor);
   COL_VAR(pOut->json(1));
}
