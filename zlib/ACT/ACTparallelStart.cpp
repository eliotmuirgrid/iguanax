// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: ACTparallelStart
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Sunday 18 September 2022 - 12:37PM
// ---------------------------------------------------------------------------

#include <ACT/ACTparallelStart.h>

#include <SCK/SCKloop.h>
#include <COL/COLthreadPool.h>
#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// Back on MAIN - passing COLstring by value so we don't have to worry about ownership issues.
void ACTthreadDone(int* pCount, COLstring Error, COLclosure1<const COLstring&>* pCallback){
   COL_FUNCTION(ACTthreadDone);
   static COLstring ErrorList;  // TODO - this makes this code something that can only be used once
   // Also it wastes a bit of memory since ErrorList persists after the call - would be better to make it
   // on the heap and deallocate at the end.
   int& Count = *pCount;
   COL_VAR(Count);
   ErrorList += Error;
   Count--;
   if (Count == 0){
      delete pCount;
      if (!ErrorList.is_null()){
         COL_ERR("Errors:" << newline << ErrorList);
      }
      COL_TRC("Action complete!  On to the next thing");
      pCallback->runAndDelete(ErrorList);
   }
}

// on THREAD
void ACTthreadAction(SCKloop* pLoop, COLclosure0* pClosure, int* pCount, COLclosure1<const COLstring&>*  pCallback){
   COL_FUNCTION(ACTthreadAction);
   // try catch and post back to main thread and bring error to the main thread so we know what went wrong
   // NOTICE WE NEVER MODIFY pCount except in the main thread.  Share nothing
   try{
      pClosure->runAndDelete();
      pLoop->post(COLnewClosure0(&ACTthreadDone, pCount, COLstring(), pCallback));
   } catch (const COLerror& Error){
      COLstring ErrorString = Error.description();
      pLoop->post(COLnewClosure0(&ACTthreadDone, pCount, ErrorString, pCallback)); // passing COLstring by value so we don't have to worry about ownership issues.
   }
}

// on MAIN thread
void ACTparallelStart(SCKloop* pLoop, COLlist<COLclosure0*>* pActions, COLclosure1<const COLstring&>* pCallback){
   COL_FUNCTION(ACTparallelStart);
   COLlist<COLclosure0*>& Actions = *pActions;  // We do implicitly modify it.
   int* pCount = new int;
   *pCount = Actions.size();
   COL_TRC("Launching " << *pCount << " actions in parallel.");
   for (auto i=Actions.begin(); i != Actions.end(); i++){
      pLoop->threadPool()->scheduleTask(COLnewClosure0(&ACTthreadAction, pLoop, *i, pCount, pCallback));
   }
   pActions->clear();  // to make it clear we alter it.
}