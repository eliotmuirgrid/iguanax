// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITclonePermission
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Friday 05 May 2023 - 12:50PM
// ---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <GIT/GITclonePermission.h>
#include <GITU/GITUconfigFlag.h>
#include <PRO/PROexecute.h>
COL_LOG_MODULE;

bool GITclonePermission(const COLstring& Username, const COLstring& GitUrl, COLstring* pError){
   COL_FUNCTION(GITclonePermission);
   COLstring Command = "git"+ GITUconfigFlag(Username) +" ls-remote " + GitUrl;
   COL_VAR(Command);
   COLstring Out;
   int ExitCode = PROexecuteString("", Command, &Out, pError);
   COL_VAR3(ExitCode, Out, *pError);
   return ExitCode == 0;
}
