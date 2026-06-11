// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testVersion
//
// Description:
//
// Implementation
//
// Author: Orkhan Amikishiyev
// Date:   Thursday 02 February 2023 - 06:01AM
// ---------------------------------------------------------------------------
#include "testVersion.h"

#include <GIT/GITversion.h>
#include <UNIT/UNITapp.h>
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>    // for SetEnvironmentVariable
#endif

#include <COL/COLlog.h>
COL_LOG_MODULE;

// assume GIT is present
static void testFindVersion() {
   COL_FUNCTION(testFindVersion);
   COLstring Version, VersionErr;
   bool Found = GITversion(&Version, &VersionErr);
   bool VersionFound = Version.toLowerCase().find("version") != npos;
   COL_VAR4(Found, VersionFound, Version, VersionErr);
   COL_DUMP("Version:", Version.c_str(), Version.size());
   UNIT_ASSERT_EQUALS(VersionFound, true);
   UNIT_ASSERT_EQUALS(Found, true);
}

 // Have to comment this one out since we cannot have environmental variables set per thread.
 // https://stackoverflow.com/questions/69863120/set-different-environment-variables-per-thread
static void testNoGitVersion(){
   COL_FUNCTION(testNoGitVersion);
#ifdef _WIN32
   SetEnvironmentVariable("PATH", "");
#else
   setenv("PATH", "", 1);  // We deliberately break the PATH so that we cannot find GIT.
#endif
   COLstring Version, VersionErr;
   bool Found = GITversion(&Version, &VersionErr);
   COL_VAR3(Found, Version, VersionErr);
}

void testVersion(UNITapp* pApp){
   COL_FUNCTION(testVersion);
   pApp->add("git/version",     &testFindVersion);
   // pApp->add("git/notpresent", &testNoGitVersion);  // Have to comment this one out since we cannot have environmental variables set per thread.
}
