//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testApp
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tue 18 Nov 2014 10:23:15 EST
//---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>

#include <COL/COLutils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testSimple(){
   COL_FUNCTION(testSimple);
}

static void testComplex(){
   COL_FUNCTION(testComplex);
}

static void testSlow(){
   COL_FUNCTION(testSlow);
   COLsleep(500);
}

static void testError(){
   COL_FUNCTION(testError);
   //Comment out so it's not annoying in the build system.
   //throw COLerror("Rubber band broke.", COLerror::PreCondition);
}

static void testClosure(COLstring Name){
   COL_FUNCTION(testClosure);
   UNIT_ASSERT_EQUALS("Expected", Name);
}


void testApp(UNITapp* pApp){
   pApp->add("app/simple", &testSimple);
   pApp->add("app/complex", &testComplex);
   pApp->add("app/slow", &testSlow);
   pApp->add("app/error", &testError);
   pApp->addClosure("app/closure", COLnewClosure0(&testClosure, COLstring("Expected")));
}

