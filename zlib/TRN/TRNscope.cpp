//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNscope
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Tuesday, October 4th, 2011 @ 03:45:01 PM
//---------------------------------------------------------------------------
#include <TRN/TRNscope.h>

#include <LUACX/LUAXloops.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// TODO if we want the GUI to show which iterations have "changed", then we'll
// need LUAXscope to have a separate m_IterationIsUnknown member, instead of
// this special value (which makes us lose the original target iteration).


void TRNlineAddAnnotation(TRNline* pLine, const COLstring& Data){
   COL_FUNCTION(TRNlineAddAnnotation);
   pLine->m_Count++;
   if (pLine->m_Count > 30){
      return;
   }
   if (pLine->m_Count == 30){
      COL_TRC("Too many statements on one line.  This hack means we return.");
      pLine->m_Data.append("...(truncated)", 14);
      return;
   }
   if (pLine->m_Data.is_null()){
      pLine->m_Data = Data;
   } else {
      pLine->m_Data.append("  ", 2).append(Data);
   }
}

void TRNlineAddReturn(TRNline* pLine, const COLstring& Data){
   COL_FUNCTION(TRNlineAddReturn);
   if (pLine->m_Count > 29){
      COL_TRC("Too many statements on one line.  This hack means we return.");
      return;
   }
   pLine->m_Count++;
   pLine->m_Data.append(Data);
}

// TODO - do we really need this now?
void LUAXscopeInit(LUAXscope* pTrnScope){
   COL_FUNCTION(LUAXscopeInit);
   for (int i = 0, n = pTrnScope->m_Scopes.size(); i < n; i++){
      COL_VAR(i);
      LUAXscopeInit(&pTrnScope->m_Scopes[i]);
   }
   pTrnScope->m_Initialized = true;
}

void LUAXscopeResetIterations(LUAXscope* pScope){
   COL_FUNCTION(LUAXscopeResetIterations);
   COL_DBG("Resetting iterations for scope at line " << pScope->m_Start);
   pScope->m_CurrentIteration = 0;
   pScope->m_IterationOver = true;
   for (int i = 0, n = pScope->m_Scopes.size(); i < n; i++){
      LUAXscopeResetIterations(&pScope->m_Scopes[i]);
   }
}

int LUAXscopeIncIteration(LUAXscope* pScope){
   COL_FUNCTION(LUAXscopeIncIteration);
   pScope->m_IterationOver = false;
   pScope->m_CurrentIteration++;
   COL_VAR2(pScope->m_CurrentIteration, pScope->m_TargetIteration); 
   if (pScope->m_TargetIteration == TRN_UNKNOWN_TARGET_ITERATION){
      // Unknown iteration means we have to reset child iteration numbers,
      // and clear all annotations we've found so far.
      COL_TRC("Target is unknown iteration");
      pScope->m_Lines.clear();
      for (int i = 0, n = pScope->m_Scopes.size(); i < n; i++){
         LUAXscopeResetIterations(&pScope->m_Scopes[i]);
         pScope->m_Scopes[i].m_Lines.clear();
      }
   } else if (pScope->m_CurrentIteration <= pScope->m_TargetIteration){
      COL_TRC("New iteration so reset child scopes");
      // New iteration means all child scopes should be reset.
      for (int i = 0, n = pScope->m_Scopes.size(); i < n; i++){
         LUAXscopeResetIterations(&pScope->m_Scopes[i]);
      }
   }
   COL_VAR(pScope->m_CurrentIteration);
   return pScope->m_CurrentIteration;
}

void LUAXscopeSetChildTargetsToUnknown(LUAXscope* pScope){
   for (int i = 0, n = pScope->m_Scopes.size(); i < n; i++){
      pScope->m_Scopes[i].m_TargetIteration = TRN_UNKNOWN_TARGET_ITERATION;
      LUAXscopeSetChildTargetsToUnknown(&pScope->m_Scopes[i]);
   }
}

void LUAXscopeSetChildTargetsToCurrentIterations(LUAXscope* pScope){
   COL_FUNCTION(LUAXscopeSetChildTargetsToCurrentIterations);
   for (int i = 0, n = pScope->m_Scopes.size(); i < n; i++){
      LUAXscope& ChildScope = pScope->m_Scopes[i];
      ChildScope.m_TargetIteration=COL_MAX(ChildScope.m_CurrentIteration, 1);
      LUAXscopeSetChildTargetsToCurrentIterations(&ChildScope);
   }
}

void LUAXscopeReset(LUAXscope* pScope){
   COL_FUNCTION(LUAXscopeReset);
   LUAXscopeResetIterations(pScope);
   pScope->m_IterationOver = true;
   pScope->m_MostRecentLine = -1;
   for (int i = 0, n = pScope->m_Scopes.size(); i < n; i++){
      LUAXscopeReset(&pScope->m_Scopes[i]);
   }
   pScope->m_Lines.clear();
}

TRNline* LUAXscopeOnLine(LUAXscope* pScope, const int Line, bool CurrentIterationSoFar, bool IsRootFunctionCall, LUAXscope** ppScope){
   COL_FUNCTION(LUAXscopeOnLine);
   COL_VAR3(Line, CurrentIterationSoFar, IsRootFunctionCall);
   if (pScope->m_IterationOver && CurrentIterationSoFar){
      if (pScope->m_CurrentIteration == 0){
        pScope->m_MostRecentLine = pScope->m_Start; 
      } 
      LUAXscopeIncIteration(pScope);
   }
   // If IsRootFunctionCall, then we already know we're on the correct function call (onLine() wouldn't get called otherwise).
   CurrentIterationSoFar &= (pScope->m_TargetIteration == pScope->m_CurrentIteration || pScope->m_TargetIteration == TRN_UNKNOWN_TARGET_ITERATION || IsRootFunctionCall);
   for (int i = 0, n = pScope->m_Scopes.size(); i < n; i++){
      COL_VAR(i);
      if (LUAXscopeLineInRange(pScope->m_Scopes[i],Line)){
         pScope->m_MostRecentLine = Line; // This helps us know that we've "moved on" in the current iteration.
         return LUAXscopeOnLine(&pScope->m_Scopes[i], Line, CurrentIterationSoFar, false, ppScope);
      }
   }
   // If we haven't returned, Line is in this scope //
   if (ppScope) {
      *ppScope = pScope;
   }
   pScope->m_MostRecentLine = Line;
   
   if (CurrentIterationSoFar){
      COL_TRC("Returning line " << Line);
      return &pScope->m_Lines[Line];
   } else {
      COL_TRC("Returning NULL");
      return NULL;
   }
}

void LUAXscopeOutputAnnotations(const LUAXscope& Scope, COLostream& Stream){
   COL_FUNCTION(LUAXscopeOutputAnnotations);
   COLhashmapPlace LinePlace;

   COL_VAR2(Scope.m_CurrentIteration, LUAXscopeTargetIteration(Scope));
   if (Scope.m_CurrentIteration < LUAXscopeTargetIteration(Scope)){
      COL_TRC("Unable to reach the target iteration of this scope");
      // We didn't actually reach the target iteration of this scope.
      // Annotations will be empty, but we can print out something more
      // helpful than empty lines.
      for (int i = Scope.m_Start, n = Scope.m_End; i <= n; i++){
         Stream << COL_T("<pre class=\'TRNline_") << i << COL_T("\'>");
         if (i == Scope.m_Start) {
            Stream << COL_T("Requested iteration was not reached.  Try re-running the script.");
         }
         Stream << COL_T(" </pre>"); // the space is important - without it browsers won't give the <pre> its own line.
      }
   } else {
      COL_TRC("Iteration was reached, we have annotations to print.");
      for (int i = Scope.m_Start, n = Scope.m_End; i <= n; i++){
         // EndOfLoop is true iff
         //  1) the scope is a loop,
         //  2) i is the last executed line in the scope, and
         //  3) the target iteration is the last iteration of the loop.
         bool EndOfLoop = (Scope.m_Type == LUAXscope::LOOP && i == Scope.m_MostRecentLine && Scope.m_TargetIteration == Scope.m_CurrentIteration);
         COL_VAR(EndOfLoop);
         Stream << COL_T("<pre class=\'TRNline_") << i << COL_T("\'>");

         LinePlace = Scope.m_Lines.find(i);
         if (LinePlace){ // if line in scope
            const COLstring& LineAnnotations = Scope.m_Lines[LinePlace].m_Data;
            COL_VAR(LineAnnotations);
            Stream << LineAnnotations;
            if (EndOfLoop && !LineAnnotations.is_null()) Stream << COL_T(", ");
         }
         if (EndOfLoop) Stream << COL_T("<span class=\'lc\'>-- exit loop</span>");
         Stream << COL_T(" </pre>"); // the space is important - without it browsers won't give the <pre> its own line.
      }
   }
}

bool LUAXscopeLineInRange(const LUAXscope& Scope, int Line){
   return Scope.m_Start <= Line && Line <= Scope.m_End;
}
