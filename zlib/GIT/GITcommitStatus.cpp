// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITcommitStatus
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Wednesday 31 July 2024 - 11:04AM
// ---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <GIT/GITcommitStatus.h>
#include <PRO/PROexecute.h>
COL_LOG_MODULE;

bool GITcommitStatus(const COLstring& Dir, const COLstring& Args, COLstring* pOut, COLstring* pError){
   COL_FUNCTION(GITcommitStatus);
   COLstring Command = "git diff-tree --no-commit-id --name-status -r " + Args + " --root";
   COL_VAR(Command);
   int ExitCode = PROexecuteString(Dir, Command, pOut, pError);
   COL_VAR3(ExitCode, *pOut, *pError);
   if (ExitCode != 0){
      COL_ERR(Command << " failed" << " Dir = " << Dir);
   }
   return ExitCode == 0;
}