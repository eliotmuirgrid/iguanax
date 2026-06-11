// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITSinstall
//
// Description: Implementation
//
// GIT install
//
// Author: Matthew Sobkowski
// Date:   Monday 10 April 2023
// ---------------------------------------------------------------------------
#include <GITS/GITSinstall.h>
#include <GITS/GITSinstallInfo.h>

#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <PRO/PROexecute.h>
#include <PRO/PROenv.h>
#include <PRO/PROwhich.h>

#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static bool GITSaddToPath(COLstring* pError) {
   COL_FUNCTION(GITSaddToPath);
   COLstring PathToGit = "C:\\Program Files\\Git\\cmd";
   COLstring PathEnv   = PROgetenv("PATH");
   COL_VAR(PathEnv);
   PathEnv = PathEnv + ";" + PathToGit;
   PROsetenv("PATH", PathEnv);
   COLstring GitPath;
   bool Exists = PROwhich(&GitPath, "git");
   COL_VAR2(Exists, GitPath);
   if(!Exists) { *pError = "Error adding '" + PathToGit + "' to system path"; }
   return Exists;
}

bool GITSinstallImpl(COLstring* pError) {
   COL_FUNCTION(GITSinstallImpl);
   COLstring InstallerPath = GITSinstallerFile();
   COLstring GitPath;
   bool Exists = PROwhich(&GitPath, "git");
   if(Exists) {
      COL_TRC("Git already exists in '" + GitPath + "' -- no need to install");
      return true;
   }
   COL_VAR2(Exists, GitPath);
#ifdef _WIN32
   if(!FILfileExists(InstallerPath)) {
      *pError = InstallerPath + " not found";
      return false;
   }
   COL_TRC("Running " << InstallerPath);
   COLstring Out, Err;
   int       ExitCode = PROexecuteString("", InstallerPath + " /VERYSILENT /SUPPRESSMSGBOXES", &Out, &Err, 10000);
   COL_VAR3(ExitCode, Out, Err);
   *pError = Err;
   if(ExitCode != 0) { return false; }
   return GITSaddToPath(pError);
#else
   COL_TRC("Git not found on posix? This is bad");
   *pError = "Git not found on path";
   return false;
#endif
}

// /git/install
void GITSinstall(const COLwebRequest& Request) {
   COL_FUNCTION(GITSinstall);
   COLstring Error;
   if (getenv("GITS_FAKE_NOT_PRESENT")){
      COL_ERR("We fake install git because GITS_FAKE_NOT_PRESENT is set.");
      PROunsetenv("GITS_FAKE_NOT_PRESENT");
      COLrespondSuccess(Request.Address);
      return;
   }

   bool      Success = GITSinstallImpl(&Error);
   COL_VAR2(Success, Error);
   Success ? COLrespondSuccess(Request.Address) : COLrespondError(Request.Address, Error);
}