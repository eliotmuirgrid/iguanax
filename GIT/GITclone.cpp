// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITclone
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday 13 February 2023 - 09:53AM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GIT/GITclone.h>
#include <GIT/GITcheckoutBranch.h>
#include <GIT/GITclean.h>
#include <GIT/GITclonePermission.h>
#include <GIT/GITbranch.h>
#include <GIT/GITexecute.h>
#include <GIT/GITreset.h>
#include <GITU/GITUconfigFlag.h>
#include <GIT/GITinit.h>
#include <GITU/GITUurl.h>
#include <GITU/GITUutils.h>
#include <PRO/PROexecute.h>
COL_LOG_MODULE;

bool GITinitializeCachedRepo(const COLstring& Username, const COLstring& Url, const COLstring& Branch,
                             const COLstring& NewRepoLocation, bool& UseSsh, COLstring* pOut, COLstring* pError) {
   COL_FUNCTION(GITinitializeCachedRepo);
   COL_VAR2(Url, NewRepoLocation);
   GITUurl GitUrl;
   if (!GITUparseUri(Url, &GitUrl, pError)){
      (*pError) = "Could not parse provided git url. " + *pError;
      return false;
   }
   bool Success = false;
   // IX-1690 - We check for permission before cloning as a failed clone will temporarily create a repository.
   // This messes with the logic checking for whether we have templates cached already (showing we have a repo even if we should not have access to it).
   if (!UseSsh){
      COL_TRC("Attempting to clone using http(s).");
      COLstring WebUrl;
      GITUhttpxUrl(Url, &WebUrl);
      // We do not need to provide a Username as that is needed for ssh key configuration
      Success = GITclonePermission("", WebUrl, pError) && GITclone("", WebUrl, NewRepoLocation, pOut, pError);
   }

   if (!Success) {
      COL_TRC("Attempting to clone using ssh.");
      UseSsh = true;
      COLstring SshUrl;
      GITUsshUrl(Url, &SshUrl);
      pError->clear();
      Success = GITclonePermission(Username, SshUrl, pError) && GITclone(Username, SshUrl, NewRepoLocation, pOut, pError);
   }
   
   if(!Success) {
      COL_TRC("Could not clone repository.");
      return false;
   }
   
   if(!Branch.is_null() && !GITcheckoutBranch(NewRepoLocation, Branch, Username, pError)) {
      COL_TRC("Could not checkout to branch " << Branch);
      FILremoveFullDirNewSafe(NewRepoLocation, pError);
      *pError = "Could not checkout to branch " + Branch + ". Repository removed: " + NewRepoLocation + ". " + *pError;
      COL_ERR(*pError);
      return false;
   }
   GITupdateDefaultBranch(NewRepoLocation);
   return GITinitConfig(Username, NewRepoLocation, true, pError);
}

static bool GITrefreshCachedRepoHardReset(const COLstring& Username, const COLstring& RepoLocation,
                                          COLstring* pOut, COLstring* pError) {
   COL_FUNCTION(GITrefreshCachedRepoHardReset);
   COL_VAR2(Username, RepoLocation);
   COLstring Out, Error;
   const COLstring FetchCommand = "git" + GITUconfigFlag(Username) + " fetch --prune";
   if(GITexecuteString(RepoLocation, FetchCommand, &Out, &Error) != 0) {
      *pOut = Out;
      *pError = Error;
      return false;
   }
   if(!GITreset(RepoLocation, "@{u}", &Out, &Error)) {
      *pOut = Out;
      *pError = Error;
      return false;
   }
   if(!GITclean(RepoLocation, &Out, &Error)) {
      *pOut = Out;
      *pError = Error;
      return false;
   }
   GITupdateDefaultBranch(RepoLocation);
   pOut->clear();
   pError->clear();
   return true;
}

bool GITrefreshCachedRepo(const COLstring& Username, const COLstring& Url, const COLstring& Branch,
                          const COLstring& RepoLocation, bool& UseSsh, COLstring* pOut, COLstring* pError) {
   COL_FUNCTION(GITrefreshCachedRepo);
   COL_VAR4(Url, Branch, Username, RepoLocation);
   (void)Url;
   (void)Branch;
   (void)UseSsh;
   return GITrefreshCachedRepoHardReset(Username, RepoLocation, pOut, pError);
}
bool GITclone(const COLstring& Username, const COLstring& GitSourceLocation, const COLstring& Dir, COLstring* pOut,
              COLstring* pError) {
   COL_FUNCTION(GITclone);
   const COLstring Source      = GITUquoteArgument(GitSourceLocation);
   const COLstring Destination = FILpathSepToPosix(GITUquoteArgument(Dir));
   const COLstring Command     = "git" + GITUconfigFlag(Username) + " clone --config core.autocrlf=false " + Source + " " + Destination;
   COL_VAR(Command);
   const int ExitCode = PROexecuteString("", Command, pOut, pError);
   if(ExitCode != 0) {
      COL_ERR(Command << " failed -- " << " Source= " << GitSourceLocation << " Dir=" << Dir);
      COL_ERR(*pError);
   }
   return ExitCode == 0;
}
