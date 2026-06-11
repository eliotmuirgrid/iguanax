// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testCopy
//
// Description:
//
// Implementation of tests
//
// Author: Eliot Muir 
// Date:   Wednesday 09 October 2024 - 12:49PM
// ---------------------------------------------------------------------------
#include "testCopy.h"

#include <UNIT/UNITapp.h>

#include <EDI/EDIgrammar.h>
#include <EDI/EDIcopy.h>
#include <EDI/EDIload.h>

#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;



static void testSimpleCopy(){
   COL_FUNCTION(testSimpleCopy);
   EDIcollection Lib, Copy;
   COLstring Error;
   bool Success = EDIloadWeb("../Iguana/app/web/HL7/HL7_2_7.js", &Lib, &Error);
   COL_VAR(*Lib.Nodes["MSH"]);
   EDIcopy(Lib, &Copy, "MSH");
   COL_VAR(Copy);
}

void testCopy(UNITapp* pApp) {
   pApp->add("copy/simple", &testSimpleCopy);
}
