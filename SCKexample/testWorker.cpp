//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testThreading 
//
// Description:
//
// Integration level test for demonstration of the SCKloop with multi-thread-
// ing. It is highly recommended that you trace the tests in this module using
// the colorized trace .collog file.
//
// Run this with tracing on SCKloop ant testThreading, e.g.
//
// ./SCKexample --test "testThreading/[*]" --t "SCKloop testThreading"
//
// Author: Trevor Hilton
// Date:   17-July-2019 
//---------------------------------------------------------------------------

#include "testWorker.h"
#include <UNIT/UNITapp.h>
#include <SCK/SCKloop.h>
#include <SCK/SCKworker.h>
#include <COL/COLvar.h>
#include <COL/COLthreadPool.h>
#include <COL/COLclosure.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void worker(COLvar* pResult){
   COL_FUNCTION(worker);
   (*pResult)["hello"] = "from worker";
}

static void resultOnMain(SCKprocessResult Result, SCKloop* pLoop){
   COL_FUNCTION(resultOnMain);

   COL_VAR(Result.ExitCode);
   COL_VAR(Result.StandardOut);
   pLoop->shutdown();
}

void testClosure(){
   COL_FUNCTION(testClosure);
   COLthreadPool ThreadPool;
   ThreadPool.start();
   SCKloop Loop(&ThreadPool);
   
   SCKexecute(&Loop, "pwd", "", 10, COLnewClosure1(resultOnMain, &Loop));

   COL_TRC("We're starting the loop...");
   Loop.start();
   COL_TRC("...the loop was shutdown and is done.");
}

void testWorker(UNITapp* pApp) {
   pApp->add("worker/closure",&testClosure);
}
