//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testSetup
//
// Description:
//
// Implmentation
//
// Author: Eliot Muir
// Date:  Wed  1 Feb 2023 10:54:35 EST
//---------------------------------------------------------------------------
#include "testSetup.h"

#include <UNIT/UNITapp.h>

#include <PRO/PROwhich.h>
#include <PRO/PROexecute.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testFindGit() {
   COL_FUNCTION(testFindGit);
   COLstring Path;
   bool Found = PROwhich(&Path, "git");
   COL_VAR2(Found, Path);
#ifdef __APPLE__
   UNIT_ASSERT_EQUALS(Path, "/usr/bin/git");   
#endif
   UNIT_ASSERT(Found);
}

static void testFindBad() {
   COL_FUNCTION(testFindBad);
   COLstring Path;
   bool Found = PROwhich(&Path, "badtotheboneLife");
   COL_VAR2(Found, Path);
   UNIT_ASSERT_EQUALS(false, Found);
}

static void testSSH(){
   COL_FUNCTION(testSSH);
   COLstring Out, Err;
   int ReturnCode = PROexecuteString("", "ssh -o \"StrictHostKeyChecking=no\" -T eliotmuir@bitbucket.org", &Out, &Err);
   COL_VAR2(Out,Err);
}


void testSetup(UNITapp* pApp){
   COL_FUNCTION(testSetup);
   pApp->add("find/git",     &testFindGit);
   pApp->add("find/bad",     &testFindBad);
   pApp->add("find/ssh", &testSSH);
}
