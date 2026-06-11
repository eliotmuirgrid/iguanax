// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITremoveTracking
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Thursday 31 October 2024 - 09:22AM
// ---------------------------------------------------------------------------

#include <GIT/GITremoveTracking.h>
#include <GIT/GITexecute.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

bool GITremoveTracking(const COLstring& Dir, const COLstring& File, COLstring* pError){
   COLstring Command = "git rm -r --cached " + File;
   COL_VAR(Command);
   COLstring Out;
   const int ExitCode = GITexecuteString(Dir, Command, &Out, pError);
   bool Success = ExitCode == 0;
   COL_VAR4(Success, ExitCode, Out, *pError);
   return Success;

}

