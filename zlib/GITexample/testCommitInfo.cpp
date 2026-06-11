// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testCommitInfo
//
// Description:
//
// Implementation of tests
//
// Author: Eliot Muir 
// Date:   Monday 27 March 2023 - 01:37PM
// ---------------------------------------------------------------------------
#include "testCommitInfo.h"

#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <FIL/FILutils.h>
#include <GIT/GITbranch.h>
#include <GITU/GITUcommitInfo.h>
#include <PRO/PROexecute.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;

static void testSimple(){
   COL_FUNCTION(testSimple);
   COLstring Slug = "commit-info-test";
   COLstring RemoveError;
   if (FILfileExists(Slug) && !FILremoveFullDirNewSafe(Slug, &RemoveError)){
      COL_ERR("Failed to clean up old test data. " + RemoveError);
      UNIT_ASSERT(false);
   }
   COLstring Out, Err;
   int ExitCode = PROexecuteString("", "git clone git@bitbucket.org:interfaceware/" + Slug, &Out, &Err);
   COL_VAR3(ExitCode, Out, Err);
   COLvar Result; 
   GITUcommitInfo(Slug, &Result);
   UNIT_ASSERT_EQUALS("Eliot Muir", Result["author"]);
   UNIT_ASSERT_EQUALS("834119f", Result["commit"]);
   UNIT_ASSERT_EQUALS("Unit tests are vital for reliable software.", Result["message"]);
   UNIT_ASSERT_EQUALS("master", Result["branch"]);
   UNIT_ASSERT(Result.exists("time_stamp"));
   if (!FILremoveFullDirNewSafe(Slug, &RemoveError)){
      COL_ERR("Failed to clean up " + Slug + " directory. " + RemoveError);
      UNIT_ASSERT(false);
   }
}

void testCommitInfo(UNITapp* pApp) {
   pApp->add("git/commit/info", &testSimple);
}
