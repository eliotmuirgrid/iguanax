// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLioCheck
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Friday 13 January 2023 - 10:03AM
// ---------------------------------------------------------------------------

#include <COL/COLioCheck.h>
#include <COL/COLerror.h>
#include <COL/COLthread.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

COLthreadID s_COLeventLoopThreadId = 0;
void COLsetMainThread() {
   COL_FUNCTION(COLsetMainThread);
   s_COLeventLoopThreadId = COLthreadGetCurrentThreadID();
   COL_VAR(s_COLeventLoopThreadId);
}

bool COLisMainThread() {
   COL_FUNCTION(COLisMainThread);
   if (s_COLeventLoopThreadId == 0) {
      COL_WRN("Main thread is not SET. return false");
      return false;
   }
   return s_COLeventLoopThreadId == COLthreadGetCurrentThreadID();
}

void COLclearMainThread(){
   COL_FUNCTION(COLclearMainThread);
   s_COLeventLoopThreadId = 0;
}

void COLassertIfMainThread() {
   COL_FUNCTION(COLassertIfMainThread);
   if(s_COLeventLoopThreadId == 0) { return; }
   COLthreadID CurrentThread = COLthreadGetCurrentThreadID();
   if(CurrentThread == s_COLeventLoopThreadId) {
      COL_ERR("SERIOUS PROBLEM: IO on main event loop thread: " << COLbacktrace());
      //throw COLerror("Running op on main thread " + COLbacktrace(), 0);
   }
}
