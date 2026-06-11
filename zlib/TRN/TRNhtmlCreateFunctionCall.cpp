// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNhtmlCreateFunctionCall
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 07 June 2023 - 08:59AM
// ---------------------------------------------------------------------------

#include <TRN/TRNhtmlCreateFunctionCall.h>
#include <TRN/TRNfunction.h>
#include <TRN/TRNargumentFormat.h>
#include <TRN/TRNfile.h>

#include <SFI/SFIhtmlEscape.h>

#include <LUA/LUAheader.h>
#include <LUA/LUAutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;


void TRNstreamFcout(
   COLostream& Stream,
   const COLstring& DisplayText,
   const char* ElementClass, // "fc" for function calls, "fr" for function returns.
   TRNfunctionCall* pCaller,
   TRNfunctionCall* pCallee,
   int CallerLine,
   const COLstring& FunctionAddress
){
   //COL_FUNCTION(TRNstreamFcout);
   const COLstring& FileName = (pCaller->m_pFunction ? pCaller->m_pFunction->m_pFile->m_Label : "");
   COLstring EscapedFileName;
   SFIhtmlEscape(FileName.c_str(), FileName.size(), EscapedFileName);

   // The "div" in here is present in the DOM, but hidden by the CSS on the client side.
   COLstring FunctionStartLine = (pCaller->m_pFunction ? COLintToString(pCaller->m_pFunction->m_Start) : "?");
   Stream << COL_T("<a href=\'\' class=\'") << ElementClass
      << COL_T(" fcout\' title=\'Step out to ")
      << pCaller->m_Name << " at " << EscapedFileName << ':' << CallerLine
      << COL_T("\'>") << DisplayText << COL_T("<div>")
      << EscapedFileName << ':' << FunctionStartLine << ':' << pCaller->m_Call << ':' << CallerLine << ':' << FunctionAddress
      << COL_T("</div></a>");
}


void TRNmakeCallLine(lua_State* L,
                     int CountOfParam,
                     TRNfunctionCall* pCaller,
                     TRNfunctionCall* pCallee,
                     int CallerLine,
                     const COLstring& FunctionAddress,
                     bool StepIn,
                     COLstring* pResult)
{
   COL_FUNCTION(TRNmakeCallLine);
   COLostream Stream(pResult, false);

   // Contains an inner span with details on what function to jump to when clicked. The inner span has the following contents:
   // <function file label>:<function start line #>:<callee call #>:<line # to jump to>
   if (StepIn){
      const COLstring& FileName = (pCallee->m_pFunction ? pCallee->m_pFunction->m_pFile->m_Label : "");
      COL_VAR(FileName);
      COLstring EscapedFileName;
      SFIhtmlEscape(FileName.c_str(), FileName.size(), EscapedFileName);

      COLstring FunctionStartLine = (pCallee->m_pFunction ? COLintToString(pCallee->m_pFunction->m_Start) : "?");
      // The "div" in here is present in the DOM, but hidden by the CSS on the client side.
      Stream << COL_T("<a href=\'\' class=\'fc fcin\' title=\'Step into ")
             << pCallee->m_Name << " at " << EscapedFileName << ':' << FunctionStartLine
             << COL_T("\'>") << pCallee->m_Name << COL_T("<div>")
             << EscapedFileName << ':' << FunctionStartLine << ':' << pCallee->m_Call << ':' << FunctionStartLine
             << COL_T("</div></a>(");
   } else if (pCaller) {
      TRNstreamFcout(Stream, pCallee->m_Name, "fc", pCaller, pCallee, CallerLine, FunctionAddress);
      Stream << '(';
   } else {
      Stream << pCallee->m_Name << '(';
   }

   TRNargumentsFormat(L, 1, CountOfParam, FunctionAddress, false, Stream);
   // TODO it would be nice if we could detect whether the function is a vararg one, and if so, output the vararg values.
   Stream << ')';
   COL_VAR(*pResult);
}


void TRNhtmlCreateFunctionCall(lua_State* L, int CountOfParam, TRNfunctionCall* pCaller, TRNfunctionCall* pCallee, int CallerLine, const COLstring& CalleeAddress){
   COL_FUNCTION(TRNhtmlCreateFunctionCall);
   COL_VAR3(*pCallee, CountOfParam, LUAparametersAsString(L, CountOfParam));
   if (!pCaller){ // Warning: trying to trace *pCaller here is dangerous.
      COL_TRC("No caller.");
   }
   if (TRNfunctionCallIsTargetCall(*pCallee)){
      COL_DBG("Callee annotation line for " << ((void*)pCallee->m_pFunction) << ", line " << pCallee->m_pFunction->m_Start);
      TRNline* pLine = TRNfunctionOnLine(pCallee->m_pFunction,pCallee->m_pFunction->m_Start);
      if (pLine){
         COLstring Annotation;
         TRNmakeCallLine(L, CountOfParam, pCaller, pCallee, CallerLine, CalleeAddress, false, &Annotation);
         TRNlineAddAnnotation(pLine,Annotation);
         //COL_VAR(Annotation);
      }
   } else {
      COL_TRC("NO callee annotation");
   }
   if (pCaller){
      bool IsCallerTarget = TRNfunctionCallIsTargetCall(*pCaller);
      COL_VAR(IsCallerTarget);
      if (IsCallerTarget){      
         COL_TRC("Making caller annotation line.");
         TRNline* pLine = TRNfunctionOnLine(pCaller->m_pFunction,CallerLine);
         if (pLine){
            COLstring Annotation;
            TRNmakeCallLine(L, CountOfParam, pCaller, pCallee, CallerLine, CalleeAddress, true, &Annotation);
            TRNlineAddAnnotation(pLine,Annotation);
            //COL_VAR(Annotation);
         }
      } else {
         COL_TRC("NO caller annotation");
      }
   } else {
      COL_TRC("NO caller annotation");
   }
}
