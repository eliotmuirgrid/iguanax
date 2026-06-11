//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LOGVstate.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   23/04/25 1:55 PM
//  ---------------------------------------------------------------------------
#include <CACHE/CACHEfile.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <GUID/GUIDid.h>
#include <LND/LNDdebug.h>
#include <LOGV/LOGVstate.h>
#include <LOGV/LOGVutils.h>
#include <LUA/LUAopen.h>
#include <LUA/LUAtest.h>
#include <TRN/TRNannotationState.h>
#include <TRN/TRNargumentRequestParse.h>
#include <TRN/TRNargumentResponse.h>
#include <TRN/TRNcallMain.h>
#include <TRN/TRNcompile.h>
#include <TRN/TRNconnectionCheck.h>
#include <TRN/TRNconstant.h>
#include <TRN/TRNfile.h>
#include <TRN/TRNhook.h>
#include <TRN/TRNindexFunc.h>
#include <TRN/TRNwriteFileInfo.h>
#include <TRN/TRNwriteScriptError.h>
COL_LOG_MODULE;

static void LOGVcleanup(lua_State* L) {
   COL_FUNCTION(LOGVcleanup);
   if(L) {
      lua_close(L);
      L = NULL;
   }
}

// second run to get node tree now that we have the id
static bool LOGVsetupSecondRequest(COLvar& FullArgs, COLvar& Result, COLstring* pErr) {
   COL_FUNCTION(LOGVsetupSecondRequest);
   COL_VAR(Result.json(1));
   if(Result["id"].asString().is_null()) {
      *pErr = "Parser failed to obtain id needed to generate view.";
      return false;
   }
   FullArgs.setArrayType();
   COLvar NewFullArgs;
   NewFullArgs["id"] = Result["id"].asString();
   NewFullArgs["expansion_tree"].setMapType();
   FullArgs.push_back(NewFullArgs);
   Result.reset();
   return true;
}

static bool LOGVexecuteScript(lua_State* L, TRNannotationState* pState, const COLstring& Message, COLstring& ScriptError) {
   COL_FUNCTION(LOGVexecuteScript);
   COL_TRC("We have an interpreter so run the program.");
   LNDregisterNodeCallback(L, pState);
   TRNconnectionCheckTimeRegister(L);
   TRNhookAnnotationSet(L);
   TRNresetAnnotationState(pState);
   const bool ScriptSuccess = TRNcallFunctionWithSampleData(L, "", Message, &ScriptError);
   TRNhookAnnotationUnset(L);
   return ScriptSuccess;
}

static void LOGVgenerateReport(TRNannotationState* pState, COLvar* pResult) {
   COL_FUNCTION(LOGVgenerateReport);
   TRNcalcMaxLineLength(pState->m_LuaFileList["main.lua"]);
   TRNwriteFileInfo(*pState, pState->m_LuaFileList["main.lua"], pResult);
   TRNargumentWriteResponses(pState->m_FullArgumentRequests, pResult);
   LOGVparseId(pResult);
   COL_VAR(pResult->json(1));
}

static bool LOGVrunAnnotation(lua_State*& L, TRNannotationState* pState, const COLvar& Args, const COLstring& Message,
                              COLvar* pResult, COLstring* pErr) {
   COL_FUNCTION(LOGVrunAnnotation);
   COL_VAR(Args.json(1));
   if(Args.isArray() && Args.size() > 0) {
      COL_TRC("Have full args");
      TRNargumentRequestParse(Args, &pState->m_FullArgumentRequests);
   }
   if(!L) { return false; }
   COLstring  ScriptError;
   const bool ScriptSuccess = LOGVexecuteScript(L, pState, Message, ScriptError);
   if(!ScriptSuccess) {
      TRNwriteScriptError(pResult, ScriptError, *pState, false);
      COL_TRC("Code has error = " << *pResult);
      *pErr = (*pResult)["ScriptError"]["Description"].asString();
      return false;
   }
   LOGVgenerateReport(pState, pResult);
   return true;
}

static void LOGVsetupState(lua_State*& L, TRNannotationState* pState, const COLstring& Vmd) {
   COL_FUNCTION(LOGVsetupState);
   COLstring EscapedPath = Vmd;
   EscapedPath.replace("\\", "\\\\");
   L                                 = LUAopen(LUA_ANNOTATION_MODE);
   const COLstring Script            = "function main(Data)\nhl7.parse{data=Data, vmd=\"" + EscapedPath + "\"}\n end";
   COL_VAR(Script);
   auto*           pFile             = new TRNfile();
   pFile->m_Source                   = Script;
   pFile->m_Label                    = TRN_MAIN_FILE_NAME;
   pFile->m_FileMissing              = false;
   pState->m_LuaFileList["main.lua"] = pFile;
}

static bool LOGVcreateAndCompile(lua_State*& L, const COLstring& Vmd, TRNannotationState* pState, COLstring* pErr) {
   COL_FUNCTION(LOGVcreateAndCompile);
   const COLstring Id          = GUIDnewShortId();
   const COLstring ProjectPath = FILpathAppend(DIRscratchDir("scripts"), Id);
   LOGVsetupState(L, pState, Vmd);
   LUAsetIdAndRoot(L, Id, ProjectPath);
   LUALmarkEnvironmentAsTest(L);
   TRNcompile(&L, pState, pErr);
   if(L == NULL) {
      COL_TRC("We have a compile error" << *pErr);
      return false;
   }
   COL_TRC("Compile OK");
   TRNindexFuncGlobal(L, pState->m_CFunctionList, pState->m_LFunctionNameMap);
   return true;
}

bool LOGVinitState(const COLvar& RequestData, TRNannotationState* pState, COLvar* pOut, COLstring* pErr) {
   COL_FUNCTION(LOGVinitState);
   const bool       UseDefault = COLvarRequiredBool(RequestData, "use_default");
   const COLstring& Data       = COLvarRequiredString(RequestData, "data");
   COLstring        Vmd        = COLvarRequiredString(RequestData, "vmd");
   COLvar           FullArgs   = COLvarRequiredObject(RequestData, "full_argument_requests");

   if(UseDefault) {
      if(!CACHEsetupDefaultVmd(&Vmd)) {
         COL_VAR(Vmd);
         *pErr = "Failed to setup default vmd.";
         return false;
      }
   }
   COL_VAR2(UseDefault, Vmd);
   COLstring Message;
   if(!LOGVisolateHL7Message(Data, &Message)) {
      *pErr = "Failed to find valid message in log.";
      return false;
   }

   lua_State* L = NULL;
   if(!LOGVcreateAndCompile(L, Vmd, pState, pErr)) {
      LOGVcleanup(L);
      return false;
   }
   if(!LOGVrunAnnotation(L, pState, FullArgs, Message, pOut, pErr)) {
      LOGVcleanup(L);
      return false;
   }

   if(!LOGVsetupSecondRequest(FullArgs, *pOut, pErr)) {
      LOGVcleanup(L);
      return false;
   }
   const bool Success = LOGVrunAnnotation(L, pState, FullArgs, Message, pOut, pErr);
   LOGVcleanup(L);
   COL_VAR(Success);
   return Success;
}