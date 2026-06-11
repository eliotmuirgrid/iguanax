// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testDateTime
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Wednesday 01 March 2023 - 08:43PM
// ---------------------------------------------------------------------------
#include "testDateTime.h"

#include <UNIT/UNITapp.h>
#include <SDB/SDBdateTime.h>
#include <SDB/SDBepochTime.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testSimple(){
   COL_FUNCTION(testSimple);
   // COLcout << SDBdateTimeMs(SDBtime()) << newline;
   // COLcout << SDBdateMs(SDBtime()) << newline;
}

void testDateTime(UNITapp* pApp){
   COL_FUNCTION(testSimple);
   pApp->add("datetime/simple", &testSimple);
}
