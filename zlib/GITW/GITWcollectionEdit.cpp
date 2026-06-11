// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITWcollectionEdit
//
// Description:
//
// Implementation
//
// Author: Tyler Brown

// Date:   Tuesday 09 May 2023 - 12:25PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <FIL/FILatomic.h>
#include <FIL/FILutils.h>
#include <GIT/GITclone.h>
#include <GITU/GITUcollection.h>
#include <GITU/GITUcollectionLoad.h>
#include <GIT/GITpath.h>
#include <GITU/GITUpermission.h>
#include <GITW/GITWcollectionEdit.h>
COL_LOG_MODULE;

static void GITWcollectionEditImpl(const COLstring& Username, const COLstring& GitUrl, const COLaddress& Address) {
   COL_FUNCTION(GITWcollectionEditImpl);
   const COLstring ScratchDir = GITuserScratchDir(Username, GitUrl);
   COL_VAR2(GitUrl, ScratchDir);
   COLstring Out, Error;
   int       ExitCode = -1;
   if(!GITUpermissionWrite(Username, GitUrl, ScratchDir, &ExitCode, &Error)) {
      COL_TRC("You do not have write permission. No point in initializing editable repository.");
      FILremoveFullDirNewSafe(ScratchDir, &Error);
      return COLrespondError(
          Address,
          "You cannot write to this repository. Check your connection or the permissions to this repository: " +
              GitUrl);
   }
   if(FILfileExists(ScratchDir)) {
      COL_TRC("Removing directory so that we can make a new one and know that there are no conflicts when we push");
      if(!FILremoveFullDirNewSafe(ScratchDir, &Error)) {
         return COLrespondError(Address, "Unable to remove directory: " + ScratchDir + ". " + Error);
      }
      COL_TRC("Scratch directory removed.");
   }
   bool UseSSH = true;
   if(!GITinitializeCachedRepo(Username, GitUrl, "", ScratchDir, UseSSH, &Out, &Error)) {
      COL_TRC("Unable to initialize repository.");
      FILremoveFullDirNewSafe(ScratchDir, &Error);
      return COLrespondError(Address, "Unable to initialize repository: " + GitUrl + ". " + Error + GitUrl);
   }

   COLstring CollectionFileName = ScratchDir + "collection.json";
   COLvar    Collection;
   if(FILfileExists(CollectionFileName)) { FILserializeLoadConfig(&Collection, CollectionFileName); }
   GITUcollection GitCollection;
   GITUcollectionLoad(&GitCollection, Collection, GitUrl, &Error);
   COLvar Output;
   GITUcollectionVar(GitCollection, &Output);
   COL_VAR(Output.json(1));
   if(FILfileExists(ScratchDir)) {
      COL_TRC("Removing directory to clean up our mess");
      FILremoveFullDirNewSafe(ScratchDir, &Error);  // non-fatal, swallow error
   }
   COLrespondSuccess(Address, Output);
}

// /git/collection/edit
void GITWcollectionEdit(const COLwebRequest& Request) {
   COL_FUNCTION(GITWcollectionEdit);
   const COLstring GitUrl = COLvarRequiredString(Request.Data, "git");
   GITWcollectionEditImpl(Request.User, GitUrl, Request.Address);
}
