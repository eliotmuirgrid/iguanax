// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNloop
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Saturday 29 April 2023 - 04:55PM
// ---------------------------------------------------------------------------

#include <TRN/TRNloop.h>
#include <TRN/TRNfunction.h>
#include <TRN/TRNannotationState.h>

#include <LUACX/LUAXloops.h>

#include <COL/COLlist.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#define TRN_LOOP_END_FUNCTION_NAME "_IfwLoopEnd"

void TRNloopOnEnd(COLlist<TRNframe>& Stack){
   COL_FUNCTION(TRNloopOnEnd);
   COLPRECONDITION(Stack.size() > 0);
   TRNfunctionCall* pCall = TRNcurrentFunctionCall(Stack);
   if (TRNfunctionCallIsTargetCall(*pCall) && pCall->m_pFunction){
      TRNfunctionOnCurrentLoopEnd(pCall->m_pFunction);
   }
}

bool TRNloopIsSecretFunc(const char* pName){
   COL_FUNCTION(TRNloopIsSecretFunc);
   COL_VAR(pName);
   return pName && strcmp(pName, TRN_LOOP_END_FUNCTION_NAME) == 0;
}

static const COLstring TRN_LOOP_END_DECLARE(" local function " TRN_LOOP_END_FUNCTION_NAME "() end do ");
static const COLstring TRN_LOOP_END_CALL   (" end " TRN_LOOP_END_FUNCTION_NAME "() ");

// We have to do this backwards, since the m_TokenEndPositions of later
// scopes will be thrown off by modifying earlier scopes.
// We have to pass in a boolean, and use |= to update it in this function as it is potentially recursive.
// If we add any loops, we the |= will set it to true and keep it that way regardless of how many times
// we recursively all it. See IX-1702.
static void TRNloopAddEndCall(COLstring* pCode, const LUAXscope& Scope, bool* pResult){
   COL_FUNCTION(TRNloopAddEndCall);
   if (Scope.m_Type == LUAXscope::LOOP){
      pCode->insert(Scope.m_EndTokenPosition, TRN_LOOP_END_CALL);
      *pResult |= true;
      COL_TRC("inserted end call");
   }
   for (int i = Scope.m_Scopes.size()-1; i >= 0; i--){
      TRNloopAddEndCall(pCode, Scope.m_Scopes[i], pResult);
   }
   if (Scope.m_Type == LUAXscope::LOOP){
      pCode->insert(Scope.m_StartTokenPosition, TRN_LOOP_END_DECLARE);
      *pResult |= true;
      COL_TRC("inserted end declare");
   }
}

bool TRNloopInsertEndCalls(COLstring* pCode){
   COL_FUNCTION(TRNloopInsertEndCalls);
   COLPRECONDITION(pCode);
   COL_VAR(*pCode);
   LUAXscope CodeScope;
   LUAXfindAllScopes(*pCode, CodeScope);
   bool Result = false;
   TRNloopAddEndCall(pCode, CodeScope, &Result);
   COL_VAR(Result);
   return Result;
}
