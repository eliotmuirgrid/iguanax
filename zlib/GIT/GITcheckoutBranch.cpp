//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GITcheckoutBranch.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <GIT/GITcheckoutBranch.h>
#include <GITU/GITUconfigFlag.h>
#include <COL/COLlog.h>
#include <GIT/GITexecute.h>
COL_LOG_MODULE;

bool GITcheckoutBranch(const COLstring& Dir, const COLstring& Branch, const COLstring& Username, COLstring* pError) {
   COL_FUNCTION(GITcheckoutBranch);
   COLstring Out;
   COL_VAR(Dir);

   // Ensure branch exists locally; fetch latest refs
   COLstring Command  = "git "+ GITUconfigFlag(Username)+ " fetch --all";
   int       ExitCode = GITexecuteString(Dir, Command, &Out, pError);
   COL_VAR2(Command, ExitCode);
   if(ExitCode != 0) {
      COL_VAR2(Out, *pError);
      return false;
   }

   // Discard local changes before switching
   Command  = "git reset --hard";
   ExitCode = GITexecuteString(Dir, Command, &Out, pError);
   COL_VAR2(Command, ExitCode);
   if(ExitCode != 0) {
      COL_VAR2(Out, *pError);
      return false;
   }

   // Checkout target branch
   Command  = "git checkout " + Branch;
   ExitCode = GITexecuteString(Dir, Command, &Out, pError);
   COL_VAR2(Command, ExitCode);
   if(ExitCode != 0) {
      COL_VAR2(Out, *pError);
      return false;
   }

   // Remove untracked files
   Command  = "git clean -dffx";
   ExitCode = GITexecuteString(Dir, Command, &Out, pError);
   COL_VAR2(Command, ExitCode);
   COL_VAR2(Out, *pError);
   return ExitCode == 0;
}
