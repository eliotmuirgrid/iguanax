// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testTRNcompile
//
// Description:
//
// Implementation of tests
//
// Author: Vismay Shah 
// Date:   Friday 12 May 2023 - 11:47 AM
// ---------------------------------------------------------------------------
#include "testTRNcompile.h"

#include <UNIT/UNITapp.h>

#include <TRN/TRNloop.h>

#include <COL/COLclosure.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

const char* testCodeWithLoop = "function foo()\n"
"\n"
"end\n"
"function main()\n"
"   \n"
"   for i = 1, 3 do\n"
"       trace(i)\n"
"   end\n"
"end";

const char* testCodeNoLoop = "function main()\n"
"   trace(\"Hello world\");\n"
"end";


// As part of the compile process for annotations, we need to insert special functions into the lua code to mark
// the beginning and end of loops. This test exercises that, making sure TRNloopInsertEndCalls returns whether the
// code was modified or not correctly. See bug from IX-1702.
static void testCompileLoop(){
   COL_FUNCTION(testCompileLoop);
   COL_TRC("Running function with code which has a for loop");
   COLstring ChangedCodeWithLoop = testCodeWithLoop;
   bool AnyChange = TRNloopInsertEndCalls(&ChangedCodeWithLoop);
   UNIT_ASSERT_EQUALS(true, AnyChange);
   COL_VAR2(testCodeWithLoop, ChangedCodeWithLoop);

   COL_TRC("Running function with code which has no loops");
   COLstring UnchangedCode = testCodeNoLoop;
   AnyChange = TRNloopInsertEndCalls(&UnchangedCode);
   UNIT_ASSERT_EQUALS(UnchangedCode, testCodeNoLoop);
   UNIT_ASSERT_EQUALS(false, AnyChange);
}

void testTRNcompile(UNITapp* pApp) {
   pApp->add("compile/loop",             &testCompileLoop);
}
