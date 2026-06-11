// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITcheckout
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 14 February 2023 - 01:33PM
// ---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <GIT/GITcheckout.h>
#include <GIT/GITexecute.h>
COL_LOG_MODULE;

bool GITcheckout(const COLstring& Dir, const COLstring& CommitId, COLstring* pError){
   COL_FUNCTION(GITcheckout);
   COLstring Out;
   COL_VAR(Dir);
   // Discard uncommitted changes and also remain on the same branch (git checkout command detaches the HEAD and this complicates git processes)
   COLstring Command = "git reset --hard " + CommitId;
   COL_VAR2(Dir, Command);
   int ExitCode = GITexecuteString(Dir, Command, &Out, pError);
   if( ExitCode == 0 ){
      // Need to remove any files that are untracked by git (since we are copying and pasting directories a lot of the time it brings with it untracked files)
      Command = "git clean -dffx";
      ExitCode = GITexecuteString(Dir, Command, &Out, pError);
   }
   COL_VAR3(ExitCode, Out, *pError);
   return ExitCode == 0;
}
