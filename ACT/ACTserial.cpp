// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: ACTserial
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday 19 September 2022 - 08:56AM
// ---------------------------------------------------------------------------

#include <ACT/ACTserial.h>

#include <SCK/SCKloop.h>
#include <COL/COLthreadPool.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;


void ACTexecute(ACTactionSet* pActions, SCKloop* pLoop){
   COL_FUNCTION(ACTexecute);
   if (pActions->List.empty()){
      COL_TRC("Completed actions. Calling done.");
      pActions->pDone->runAndDelete();
      delete pActions;
      return;
   }
   auto* pCurrentAction = pActions->List.front();
   pActions->List.pop_front();
   COLclosure0* pNextAction = COLnewClosure0(&ACTexecute, pActions, pLoop);
   //if (pActions->List.empty()){
   //   COL_TRC("List of actions is empty so the next action will finish it.");
   //   pNextAction = pActions->pDone;
  // } else {
   //}
   pCurrentAction->runAndDelete(pNextAction);
}

void ACTserial(ACTactionSet* pActions, SCKloop* pLoop){
   COL_FUNCTION(ACTserial);
   pLoop->addTimer(COLnewClosure0(&ACTexecute, pActions, pLoop),0);
}
