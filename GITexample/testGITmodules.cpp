// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testGITUmodules
//
// Description:
//
// Implementation of tests
//
// Author: Eliot Muir 
// Date:   Thursday 30 March 2023 - 11:09AM
// ---------------------------------------------------------------------------
#include "testGITmodules.h"

#include <COL/COLlog.h>
#include <GITU/GITUmodules.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;

static void testParseModule(){
   COL_FUNCTION(testParseModule);
   COLmap<COLstring, GITUmodule> List;
   GITUmoduleLoad(".gitmodules", &List);
   COL_VAR(List.size());
   for (auto i = List.cbegin(); i != List.cend(); i++){
      COL_VAR2(i->first, i->second.GitUrl);
   }
   UNIT_ASSERT_EQUALS("git@bitbucket.org:interfaceware/chk.git" , List["CHK"].GitUrl);
   UNIT_ASSERT_EQUALS("git@bitbucket.org:interfaceware/llpc.git", List["LLPC"].GitUrl);
}

void testGITUmodules(UNITapp* pApp) {
   pApp->add("git/modules/parse", &testParseModule);
}
