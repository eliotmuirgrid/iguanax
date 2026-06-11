// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITcommit
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Wednesday 05 April 2023 - 06:37PM
// ---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <GIT/GITcommit.h>
#include <GITU/GITUconfigFlag.h>
#include <GIT/GITexecute.h>
COL_LOG_MODULE;

static bool GITnothingStaged(const COLstring& Username, const COLstring& Dir){
   COL_FUNCTION(GITnothingStaged);
   COLstring Out, Error;
   COLstring Command = "git" + GITUconfigFlag(Username) +" diff --staged";
   COL_VAR2(Dir, Command);
	const int ExitCode = GITexecuteString(Dir, Command, &Out, &Error);
   COL_VAR3(ExitCode, Out, Error);
   // If out is null, there are no changes to commit
   return Out.is_null();
}

bool GITcommit(const COLstring& Username, const COLstring& Dir, const COLstring& Flags, const COLstring& Message, COLstring* pError){
   COL_FUNCTION(GITcommit);
   const bool AllowEmpty = COLstringContains(Flags, "--allow-empty");
   if(!AllowEmpty && GITnothingStaged(Username, Dir)) { 
      COL_TRC("No changes staged for commit, early return."); 
      return true; 
   }
   COLstring Out;
   COLstring Command = "git" + GITUconfigFlag(Username) +" commit " + Flags + " -m \"" + Message + "\"";
   COL_VAR2(Dir, Command);
   const int ExitCode = GITexecuteString(Dir, Command, &Out, pError);
   COL_VAR3(ExitCode, Out, (pError ? *pError : COLstring("")));
   if(ExitCode != 0 && !Out.is_null() && pError) { pError->append("\n" + Out); }
   return ExitCode == 0;
}