// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITfetch
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Friday 17 March 2023 - 10:43AM
// ---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <GITU/GITUconfigFlag.h>
#include <GIT/GITfetch.h>
#include <GITU/GITUutils.h>
#include <PRO/PROexecute.h>
COL_LOG_MODULE;

bool GITfetch(const COLstring& Username, const COLstring& Dir, const COLstring& SourceDir, COLstring* pOut, COLstring* pError){
   COL_FUNCTION(GITfetch);
   COLstring Command = "git" + GITUconfigFlag(Username) + " fetch " + GITUquoteArgument(SourceDir);
   COL_VAR2(Dir, Command);
   int ExitCode = PROexecuteString(Dir, Command, pOut, pError);
   COL_VAR2(*pOut, *pError);
   return ExitCode == 0;
}
