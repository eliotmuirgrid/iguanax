// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITpush
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Tuesday 25 April 2023 - 12:07PM
// ---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <GITU/GITUconfigFlag.h>
#include <GIT/GITpush.h>
#include <PRO/PROexecute.h>
COL_LOG_MODULE;

bool GITpush(const COLstring& Username, const COLstring& Dir, const COLstring& Flags, COLstring* pOut, COLstring* pError){
   COL_FUNCTION(GITpush);
   COLstring Command = "git" + GITUconfigFlag(Username) + " push";
   if (Flags != ""){ Command += " " + Flags; }
   COL_VAR(Command);
   int ExitCode = PROexecuteString(Dir, Command, pOut, pError);
   COL_VAR3(ExitCode, *pOut, *pError);
   if (ExitCode != 0){
      COL_ERR("git push failed"  << " Dir = " << Dir);
   }
   return ExitCode == 0;
}