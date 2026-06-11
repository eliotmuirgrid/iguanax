// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITinit
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Wednesday 05 April 2023 - 05:30PM
// ---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <GITU/GITUconfigFlag.h>
#include <GIT/GITcommits.h>
#include <GIT/GITexecute.h>
#include <GIT/GITinit.h>
#include <GIT/GITbranch.h>
#include <INI/INIdata.h>
#include <PRO/PROexecute.h>
#include <GITU/GITUconfig.h>
COL_LOG_MODULE;

bool GITinitEmptyCommit(const COLstring& Username, const COLstring& Dir, COLstring* pError){
   COL_FUNCTION(GITinitEmptyCommit);
   COL_VAR(Username);
   COLstring Command = "git" + GITUconfigFlag(Username) + " commit --allow-empty -m \"Initial commit\" --author=\"IguanaX <iguanax@iguanax.com>\"";
   COL_VAR(Command);
   COLstring Out;
   int ExitCode = GITexecuteString(Dir, Command, &Out, pError);
   COL_VAR3(ExitCode, Out, *pError);
   if (ExitCode != 0){
      COL_ERR(Command << " failed"  << " Dir = " << Dir);
      COL_ERR(*pError);
   }
   return ExitCode == 0;
}

bool GITinitSetBranchDefaultMain(const COLstring& Dir, COLstring* pError){
   COL_FUNCTION(GITinitSetBranchDefaultMain);
   COLstring Out;
   int ExitCode = 0;
   // Ensure local HEAD points to main
   ExitCode = PROexecuteString(Dir, "git symbolic-ref HEAD refs/heads/main", &Out, pError);
   COL_VAR3(ExitCode, Out, *pError);
   if (ExitCode != 0) return false;
   // Ensure remote-tracking origin/HEAD exists (pure local, no network)
   bool Success;
   Out.clear();
   Success = GITsetLocalDefaultBranch(Dir, "main", &Out, pError);
   COL_VAR3(Success, Out, *pError);
   return Success;
}

bool GITinitConfig(const COLstring& Username, const COLstring& Dir, bool InitialCommit, COLstring* pError){
   COL_FUNCTION(GITinitConfig);
   INIdata Config;
   if(!GITUconfigLoad(Dir, &Config, pError)){
      *pError = "Could not initialize repository: " + *pError;
      return false;
   };
   // We set these configurations this because it makes subsequent GIT operations possible.
   // We are always making copies from the cache so configuring them properly means we don't have to keep doing when we make new components or import libraries
   // Setting rebase to false means that git will perform a regular merge - it is done at the repo level instead of globally to make it more contained.
   GITUconfigSetPullRebaseFalse(&Config);
   GITUconfigSetDefaultAuthor(&Config);
   // This prevents git from converting LF to CRLF on windows
   GITUconfigSetAutoLineEndingConversion(&Config);
   // This allows for pushing to a new repository without having to specify --set-upstream. Otherwise we would need to add to the git push command a check for empty upstream etc.
   GITUconfigSetPushAutoSetupRemote(&Config);
   COL_VAR(Config);
   GITUconfigSave(Dir, Config);
   // Eliot - this seems like a bad idea...?
   // int CommitCount;
   COLstring CommitLog;
   bool Success = GITcommits(Dir, &CommitLog, pError, true, true);
   // if( Success && InitialCommit && CommitLog.is_null() ) {
   if( InitialCommit && CommitLog.is_null() ) {
      // To check for writable we perform git push --dry-run which fails if the repository does not have any commits
      // Check whether the number of commits is zero before doing an empty commit
      // but I suppose this could be done as part of isWritable - just means we would be checking for commits everytime
      COL_TRC("Initial commit is empty, performing empty commit");
      Success = GITinitEmptyCommit(Username, Dir, pError);
   }
   return Success;
}

bool GITinit(const COLstring& Username, const COLstring& Dir, bool InitialCommit, COLstring* pError){
   COL_FUNCTION(GITinit);
   COLstring Out;
   COLstring Command = "git init";
   COL_VAR2(Dir, Command);
   int ExitCode = PROexecuteString(Dir, Command, &Out, pError);
   COL_VAR3(ExitCode, Out, *pError);
   if (ExitCode != 0){
      COL_ERR("git init failed");
      return false;
   }
   if (!GITinitSetBranchDefaultMain(Dir, pError)){
      COL_ERR("Setting default branch to main failed.");
      return false;
   }
   return GITinitConfig(Username, Dir, InitialCommit, pError);
}