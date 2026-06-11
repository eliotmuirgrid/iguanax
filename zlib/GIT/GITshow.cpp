// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITshow
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Tuesday 23 May 2023 - 01:55PM
// ---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <GIT/GITshow.h>
#include <PRO/PROexecute.h>
COL_LOG_MODULE;


bool GITshow(const COLstring& Dir, const COLstring& Args, COLstring* pOut, COLstring* pError){
   COL_FUNCTION(GITshow);
   COLstring Command = "git show " + Args;
   COL_VAR(Command);
   int ExitCode = PROexecuteString(Dir, Command, pOut, pError);
   COL_VAR3(ExitCode, *pOut, *pError);
   if (ExitCode != 0){
      COL_ERR("git show failed" << " Dir = " << Dir);
   }
   return ExitCode == 0;
}