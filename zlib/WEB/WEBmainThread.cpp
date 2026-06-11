// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBmainThread
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Wednesday 24 May 2023 - 01:05PM
// ---------------------------------------------------------------------------

#include <WEB/WEBmainThread.h>

#include <COL/COLlog.h>
#include <COL/COLthread.h>
COL_LOG_MODULE;

COLthreadID s_WEBeventLoopThreadId = 0;
void WEBsetMainThread(){
   COL_FUNCTION(WEBsetMainThread);
   s_WEBeventLoopThreadId = COLthreadGetCurrentThreadID();
   COL_VAR(s_WEBeventLoopThreadId);
}

bool WEBisMainThread(COLthreadID CurrentThread){
   COL_FUNCTION(WEBisMainThread);
   COL_VAR2(CurrentThread, s_WEBeventLoopThreadId);
   return CurrentThread == s_WEBeventLoopThreadId;
}
