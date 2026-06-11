// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testGitInstall
//
// Description: Implementation
//
// Author: Matthew Sobkowski
// Date:   Wednesday 03 May 2023 - 10:37AM
// ---------------------------------------------------------------------------
#include <GITSexample/testGitInstall.h>

#include <GITS/GITSdownload.h>
#include <GITS/GITSinstall.h>

#include <UNIT/UNITapp.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// depending on build server network this could significantly delay the builds -- should mostly be used as a local test
static void testGitSetup() {
   COL_FUNCTION(testGitSetup);
#ifdef _WIN32
//   COLstring Error;
//   COLvar Path;
//   bool Download = GITSdownloadImpl(&Path, &Error);
//   COL_VAR2(Path, Error);
//   UNIT_ASSERT(Download);
//   bool Install = GITSinstallImpl("install/GITinstaller.exe", &Error);
//   COL_VAR(Error);
//   UNIT_ASSERT(Install);
#endif
}

void testGitInstall(UNITapp* pApp) {
   pApp->add("git/setup", testGitSetup);
}