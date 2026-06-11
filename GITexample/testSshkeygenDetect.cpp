// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testSshkeygenDetect
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday 08 March 2023 - 02:25PM
// ---------------------------------------------------------------------------
#include "testSshkeygenDetect.h"

#include <COL/COLlog.h>
#include <GITU/GITUsshkeygenDetect.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;

static void findSshkeyGen(){
   COL_FUNCTION(findSshkeyGen);
   COLstring Result = GITUsshkeygenDetect();
   COL_VAR(Result);
}

void testSshkeygenDetect(UNITapp* pApp){
   COL_FUNCTION(testSimple);
   pApp->add("keygen/find", &findSshkeyGen);
}
