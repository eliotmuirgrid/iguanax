// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTpermission
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Mon 14 Aug 2023 07:28:34 PDT
// ---------------------------------------------------------------------------
#ifdef _WIN32
#include <Windows.h>
#include <AclAPI.h>
#endif

#include <INST/INSTpermission.h>

#include <PRO/PROexecute.h>   // for the command version
#include <FIL/FILpathUtils.h>
#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

bool INSTgrantUserFullDirectoryControl(const COLstring& DirPath, const COLstring& User, COLstring* pError) {
   COL_FUNCTION(INSTgrantUserFullDirectoryControl);
   COL_VAR2(DirPath, User);
#ifndef _WIN32
   COL_WRN("'INSTgrantUserFullDirectoryControl' is not supported in Linux or macOS");
   return true;
#else
   DWORD Result = 0;
   PACL pOldDACL = NULL, pNewDACL = NULL;
   PSECURITY_DESCRIPTOR pSD = NULL;
   EXPLICIT_ACCESS ea;

   // Get a pointer to the existing DACL.
   Result = GetNamedSecurityInfo(DirPath.c_str(), SE_FILE_OBJECT,
         DACL_SECURITY_INFORMATION, NULL, NULL, &pOldDACL, NULL, &pSD);
   if (ERROR_SUCCESS != Result) {
      *pError = "GetNamedSecurityInfo Error: " + COLlastErrorAsString();
      COL_ERR(*pError);
      return false;
   }

   // Initialize an EXPLICIT_ACCESS structure for the new ACE.
   ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
   ea.grfAccessPermissions = FILE_ALL_ACCESS;   // "Full control"
   ea.grfAccessMode        = GRANT_ACCESS;
   ea.grfInheritance       = CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE;
   ea.Trustee.TrusteeForm  = TRUSTEE_IS_NAME;
   ea.Trustee.TrusteeType  = TRUSTEE_IS_USER;
   ea.Trustee.ptstrName    = (LPTSTR)User.c_str();

   // Create a new ACL that merges the new ACE into the existing DACL.
   if (ERROR_SUCCESS != SetEntriesInAcl(1, &ea, pOldDACL, &pNewDACL)) {
      *pError = "SetEntriesInAcl Error: " + COLlastErrorAsString();
      COL_ERR(*pError);
      LocalFree((HLOCAL) pSD);
      return false;
   }

   // Attach the new ACL as the object's DACL.
   Result = SetNamedSecurityInfo((LPTSTR)DirPath.c_str(), SE_FILE_OBJECT,
         DACL_SECURITY_INFORMATION, NULL, NULL, pNewDACL, NULL);
   bool Success = ERROR_SUCCESS == Result;
   if (!Success) {
      *pError = "SetNamedSecurityInfo Error: " + COLlastErrorAsString();
      COL_ERR(*pError);
   }
   LocalFree((HLOCAL) pSD);
   LocalFree((HLOCAL) pNewDACL);
   COL_VAR(Success);
   return Success;
#endif
}

bool INSTgrantUserFullDirectoryControlOld(const COLstring& DirPath, const COLstring& User, COLstring* pError) {
   COL_FUNCTION(INSTgrantUserFullDirectoryControlOld);
   COL_VAR2(DirPath, User);
#ifndef _WIN32
   COL_WRN("'INSTgrantUserFullDirectoryControl' is not supported in Linux or macOS");
   return true;
#else
   // The '\Q' option for the command could significant affect the speed, especially for large directories.
   COLstring Out, Command = "icacls \"" + DirPath + "\" /Q /T /grant \"" + User + "\":(OI)(CI)F";
   COL_VAR(Command);
   // The time the command takes depends on the number of files in DirPath. Hopefully 60s is long enough to cover most cases.
   int ExitCode = ExitCode = PROexecuteString(".", Command, &Out, pError, 60);
   COL_VAR3(ExitCode, Out, *pError);
   if (0 == ExitCode) { return true; }
   COLstring Error = "Failed to grant user '" + User + "' access to iguana working directory: ";
   if (ExitCode < 0) {  // handle errors produced by PROexecute, not the command
      if (PRO_TIMED_OUT == ExitCode) {
         *pError = Error + "Command timed out.";
      } else {
         *pError = Error + "Command execution error " + COLintToString(ExitCode);
      }
   } // else pError should be populated if command has a non-zero exit code.
   COL_ERR(Error << *pError);
   return false;
#endif
}
