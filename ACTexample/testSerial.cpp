//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testSerial
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tue 11 Nov 2014 14:03:22 EST
//---------------------------------------------------------------------------

#include "testSerial.h"

#include <UNIT/UNITapp.h>

#include <SCK/SCKloop.h>

#include <ACT/ACTserial.h>

#include <COL/COLthreadPool.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testAction1(COLclosure0* pDone, SCKloop* pLoop){
   COL_FUNCTION(testAction1);
   pLoop->addTimer(pDone, 0);
} 

static void testAction2(COLclosure0* pDone){
   COL_FUNCTION(testAction2);
   pDone->runAndDelete();
} 

static void testAction3(COLclosure0* pDone, bool* pRun){
   COL_FUNCTION(testAction3);
   *pRun = true;
   pDone->runAndDelete();
} 

static void allDone(SCKloop* pLoop){
   COL_FUNCTION(allDone);
   pLoop->shutdown();
}

static void testSerialAction(){
   COL_FUNCTION(testSerialAction);
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   ACTactionSet* pSet = new ACTactionSet();;
   pSet->List.add(COLnewClosure1(&testAction1, &Loop));
   pSet->List.add(COLnewClosure1(&testAction1, &Loop));
   pSet->List.add(COLnewClosure1(&testAction2));
   pSet->List.add(COLnewClosure1(&testAction1, &Loop));
   bool Run = false;
   pSet->List.add(COLnewClosure1(&testAction3, &Run));
   pSet->pDone = COLnewClosure0(&allDone, &Loop);
   COL_VAR(Run);
   ACTserial(pSet, &Loop);
   Loop.start();
   COL_TRC("Done message loop.");
   COL_VAR(Run);
   UNIT_ASSERT_EQUALS(true, Run);
}


void testSerial(UNITapp* pApp){
   pApp->add("test/action/serial", &testSerialAction);
}
