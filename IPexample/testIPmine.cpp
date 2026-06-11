// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testIPmine
//
// Description:
//
// Implementation of tests
//
// Author: Eliot Muir 
// Date:   Tuesday 26 September 2023 - 08:43AM
// ---------------------------------------------------------------------------
#include "testIPmine.h"

#include <IP/IPmine.h>

#include <UNIT/UNITapp.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testSimple(){
   COL_FUNCTION(testSimple);
   COLstring MyIp;
   bool Success = IPmine(&MyIp);
   COL_VAR2(Success, MyIp);
   UNIT_ASSERT_EQUALS(true, Success);
}

void testIPmine(UNITapp* pApp) {
   pApp->add("test/simple", &testSimple);
}
