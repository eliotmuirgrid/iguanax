//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testHealthCheck
//
// Description:
//
// Implementation
//
//  Author: Matthew Sobkowski
//  Date:   16/07/25 11:16 AM
//---------------------------------------------------------------------------
#ifdef _WIN32
#include <windows.h>
#include <Aclapi.h>
#endif

#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <UNIT/UNITapp.h>
#include <SDBA/SDBAlogDirectory.h>
#include "testDirCheck.h"
COL_LOG_MODULE;

// this is apparently the only reliable way to set a read-only directly on windows
#ifdef _WIN32
bool setReadOnly(const char* dir) {
   // Clear existing DACL to avoid inheritance and stale ACEs
   DWORD clearResult = SetNamedSecurityInfoA(
      const_cast<LPSTR>(dir),
      SE_FILE_OBJECT,
      DACL_SECURITY_INFORMATION,
      NULL, NULL, NULL, NULL);

   if (clearResult != ERROR_SUCCESS) {
      COL_TRC("SetNamedSecurityInfo (clear DACL) failed: " << clearResult << "\n");
      return false;
   }
   // Get current DACL
   PACL pOldDACL = nullptr;
   PSECURITY_DESCRIPTOR pSD = nullptr;

   DWORD result = GetNamedSecurityInfoA(
      dir, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION,
      NULL, NULL, &pOldDACL, NULL, &pSD);

   if (result != ERROR_SUCCESS) {
      COL_TRC("GetNamedSecurityInfo failed: " << result << "\n");
      return false;
   }

   // Grant minimal read-only access to EVERYONE
   EXPLICIT_ACCESSA ea = {};
   ea.grfAccessPermissions =
      FILE_LIST_DIRECTORY |
      FILE_READ_ATTRIBUTES |
      SYNCHRONIZE |
      READ_CONTROL;
   ea.grfAccessMode = GRANT_ACCESS;
   ea.grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
   ea.Trustee.TrusteeForm = TRUSTEE_IS_NAME;
   ea.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
   ea.Trustee.ptstrName = const_cast<LPSTR>("EVERYONE");

   PACL pNewDACL = nullptr;
   result = SetEntriesInAclA(1, &ea, pOldDACL, &pNewDACL);
   if (result != ERROR_SUCCESS) {
      COL_TRC("SetEntriesInAcl failed: " << result << "\n");
      LocalFree(pSD);
      return false;
   }

   result = SetNamedSecurityInfoA(
      const_cast<LPSTR>(dir),
      SE_FILE_OBJECT,
      DACL_SECURITY_INFORMATION,
      NULL, NULL, pNewDACL, NULL);

   if (result != ERROR_SUCCESS) {
      COL_TRC("SetNamedSecurityInfo failed: " << result << "\n");
   }

   LocalFree(pSD);
   LocalFree(pNewDACL);
   return result == ERROR_SUCCESS;
}
#endif


static void resetDir(const COLstring& Dir, const int Mode) {
   COL_VAR2(Dir, Mode);
   COLstring Error;
   if(FILfileExists(Dir)) { FILremoveFullDirNewSafe(Dir, &Error); }
   const COLstring LogFile = FILpathAppend(Dir, "test.log");
   FILcreateDirectoryTreeToFile(LogFile, Mode);
}

static void testDirHealth() {
   const COLstring TestDir   = FILpathAppend(FILworkingDir(), "testDir");
   const COLstring LockedDir = FILpathAppend(FILworkingDir(), "lockedDir");
   resetDir(TestDir, FIL_USER_RWX);
   resetDir(LockedDir, FIL_OTHER_RX);
#ifdef _WIN32
   setReadOnly(LockedDir.c_str());
#endif
   COLvar Out;
   SDBAdirCheck(TestDir, Out[TestDir]);
   COL_VAR(Out.json(1));
   UNIT_ASSERT(Out[TestDir]["writable"].asBool());
   SDBAdirCheck(LockedDir, Out[LockedDir]);
   COL_VAR(Out.json(1));
   UNIT_ASSERT(!Out[LockedDir]["writable"].asBool());
}

void testDirCheck(UNITapp* pApp){
   pApp->add("dir/check", &testDirHealth);
}