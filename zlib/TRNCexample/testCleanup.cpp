//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2026 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: testCleanup.cpp
//
//  Description: Implementation
//
//  Author: Codex
//  ---------------------------------------------------------------------------
#include "testCleanup.h"

#include <TRNC/TRNCcleanup.h>

#include <UNIT/UNITapp.h>

#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

namespace {
constexpr COLint32 kDirMode = FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX;

struct TestDirGuard {
   explicit TestDirGuard(const COLstring& TestDir) : m_OriginalDir(FILworkingDir()), m_TestDir(TestDir) {
      COLstring RemoveError;
      if(FILfileExists(m_TestDir)) { FILremoveFullDirNewSafe(m_TestDir, &RemoveError); }
      FILmakeFullDir(m_TestDir, kDirMode);
      FILsetWorkingDir(m_TestDir);
   }

   ~TestDirGuard() {
      FILsetWorkingDir(m_OriginalDir);
      COLstring RemoveError;
      if(FILfileExists(m_TestDir)) { FILremoveFullDirNewSafe(m_TestDir, &RemoveError); }
   }

   COLstring m_OriginalDir;
   COLstring m_TestDir;
};
} // namespace

static void testArchiveComponentIntoCompsDir() {
   COL_FUNCTION(testArchiveComponentIntoCompsDir);
   const COLstring TestDir = FILpathAppend(FILworkingDir(), "trnc.cleanup.archive.test");
   TestDirGuard     Guard(TestDir);

   const COLstring ComponentId      = "bitbucketo_testComponent";
   const COLstring ComponentDevDir  = DIRcomponentDevDir(ComponentId);
   const COLstring ComponentRootDir = DIRcomponentDir(ComponentId);
   const COLstring ArchivedCompDir  = FILpathAppend(DIRarchiveDir(), "comps/" + ComponentId + FIL_PATH_SEP_NATIVE);
   const COLstring OldArchiveDir    = FILpathAppend(DIRarchiveDir(), ComponentId + FIL_PATH_SEP_NATIVE);

   FILmakeFullDir(ComponentDevDir, kDirMode);
   FILwriteFile(FILpathAppend(ComponentDevDir, "main.lua"), "return true\n");
   FILwriteFile(DIRcomponentConfig(ComponentId), "{\"name\":\"broken\"}\n");

   UNIT_ASSERT(FILfileExists(ComponentRootDir));
   UNIT_ASSERT(!FILfileExists(ArchivedCompDir));
   UNIT_ASSERT(!FILfileExists(OldArchiveDir));

   TRNCarchiveComponent(ComponentId);

   UNIT_ASSERT(!FILfileExists(ComponentRootDir));
   UNIT_ASSERT(FILfileExists(ArchivedCompDir));
   UNIT_ASSERT(FILfileExists(FILpathAppend(ArchivedCompDir, "dev/main.lua")));
   UNIT_ASSERT(FILfileExists(FILpathAppend(ArchivedCompDir, "component.json")));
   UNIT_ASSERT(!FILfileExists(OldArchiveDir));
}

void testCleanup(UNITapp* pApp) {
   pApp->add("cleanup/archive_component_into_comps_dir", &testArchiveComponentIntoCompsDir);
}
