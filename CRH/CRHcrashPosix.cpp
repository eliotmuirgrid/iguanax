//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: CRHcrashWindows
//
// Author: John Q
//
// Description:
//
// Implementation
//---------------------------------------------------------------------------

#include "CRHcrash.h"

#include <signal.h>
#include <unistd.h>

#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

bool CRHcoresDirExistsWithRightPermissions() {
   COL_FUNCTION(CRHcoresDirExistsWithRightPermissions);
   return (FILhasPermission("/cores", F_OK) && FILhasPermission("/cores", R_OK) && FILhasPermission("/cores", W_OK));
}

bool CRHenableCrashDump(){
   COL_FUNCTION(CRHenableCrashDump);
#ifdef __APPLE__
   if (!CRHcoresDirExistsWithRightPermissions()){
      COL_TRC("/cores/ not readable and writable");
      return false;
   }
   COLstring PidFile = CRHpidFileName();

   COLstring MyPid = COLintToString(getpid());
   COL_TRC("About to write PID file.");
   COL_VAR2(PidFile, MyPid);
   FILwriteFile(PidFile, MyPid);
   COL_TRC("Written PID file.");

   // https://developer.apple.com/library/archive/technotes/tn2124/_index.html#//apple_ref/doc/uid/DTS10003391-CH1-SECCOREDUMPS
   // Programatically this is like executing ulimit -c unlimited  - which enables crash dumps
   struct rlimit   limit;
   limit.rlim_cur = RLIM_INFINITY;
   limit.rlim_max = RLIM_INFINITY;
   return setrlimit(RLIMIT_CORE, &limit) == 0;
#else // __linux__
   return false;
#endif
}
