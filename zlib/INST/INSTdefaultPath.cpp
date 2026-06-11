// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTdefaultPath
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Tuesday 08 August 2023 - 01:17PM
// ---------------------------------------------------------------------------

#include <INST/INSTdefaultPath.h>

#include <PRO/PROexecute.h>

#include <COL/COLvarUtils.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
#include <COL/COLweb.h>
COL_LOG_MODULE;

void INSTdefaultPath(const COLwebRequest& Request){
   COL_FUNCTION(INSTdefaultPath);
   COLvar Result;
   COL_VAR(Result.json(1));
   COLstring Command = "cmd.exe /C \"echo %ProgramFiles%\"";
   COLstring Out, Err;
   int ExitCode = PROexecuteString("", Command, &Out, &Err);
   Out.stripAll('\n');
   COL_VAR2(ExitCode, Err);
   if (0 == ExitCode) {
      return COLrespondSuccess(Request.Address, Out);
   }
   return COLrespondError(Request.Address, Out);
}