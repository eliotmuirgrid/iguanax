// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testGITcache
//
// Description:
//
// Implementation of tests
//
// Author: Eliot Muir 
// Date:   Saturday 01 April 2023 - 11:22AM
// ---------------------------------------------------------------------------
#include "testGITcache.h"

#include <COL/COLlog.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GITU/GITUcache.h>
#include <GIT/GITpath.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;

static bool cleanCache() {
   COLstring Error;
   if (FILfileExists(GITcacheDir()) && !FILremoveFullDirNewSafe(GITcacheDir(), &Error)){
      COL_TRC("Failed cleaning up git_cache dir");
      return false;
   }
   return true;
}

static void testCacheInit(){
   COL_FUNCTION(testCacheInit);
   UNIT_ASSERT(cleanCache());
   COLstring Log;
   bool Result = GITUcacheInit("", &Log);
   COLstring Hl7Client = FILpathAppend("git_cache", FILpathAppend("bitbucket.org", FILpathAppend("interfaceware", "hl7_client")));
   UNIT_ASSERT(FILfileExists(Hl7Client));
   COL_VAR2(Log, Result);
   COL_TRC("Do the init cache a second time.");
   Result = GITUcacheInit("", &Log);
   COL_VAR2(Log, Result);
   UNIT_ASSERT(cleanCache());
}

void testGITcache(UNITapp* pApp) {
   pApp->add("cache/init", &testCacheInit);
}
