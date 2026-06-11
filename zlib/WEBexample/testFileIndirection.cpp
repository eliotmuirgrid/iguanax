// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testFileIndirection
//
// Description:
//
// Implementation of tests
//
// Author: Eliot Muir 
// Date:   Friday 30 June 2023 - 04:59PM
// ---------------------------------------------------------------------------
#include "testFileIndirection.h"

#include <UNIT/UNITapp.h>

#include <COL/COLclosure.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testDestination(const COLstring& Result, COLuint64 ConnectionId){
   COL_FUNCTION(testDestination);
}

static void testFileIndirection(){
   COL_FUNCTION(testFileIndirection);
   COLuint64 ConnectionId = 100;
   COLclosure1<const COLstring&>* pCallback = COLnewClosure1(&testDestination, ConnectionId);
   pCallback->runAndDelete("Life");
   
}

void testFileIndirection(UNITapp* pApp) {
   pApp->add("file/indirection", &testFileIndirection);
}
