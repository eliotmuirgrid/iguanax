// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITWcollectionSave
//
// Description:
//
// Implementation
//
// Author: Tyler Brown
// Date:   Thursday 11 May 2023 - 12:19PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <FIL/FILatomic.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GIT/GITadd.h>
#include <GIT/GITclone.h>
#include <GIT/GITcommit.h>
#include <GIT/GITpath.h>
#include <GIT/GITpush.h>
#include <GIT/GITbranch.h>
#include <GITU/GITUcollection.h>
#include <GITU/GITUcollectionLoad.h>
#include <GITU/GITUpermission.h>
#include <GITU/GITUurl.h>
#include <GITU/GITUutils.h>
#include <GITW/GITWcollectionSave.h>
#include <GSV/GSVlink.h>
COL_LOG_MODULE;

typedef bool (*GITWnameValidator)(const COLstring& Name, COLstring* pError);

static bool GITWnameValidationRunner(const COLvar& Data, GITWnameValidator pValidator, COLstring* pError) {
   COL_FUNCTION(GITWnameValidationRunner);
   auto Map = Data.map();
   for(auto i = Map.cbegin(); i != Map.cend(); i++) {
      if(!(*pValidator)(i->second["name"], pError)) { return false; }
   }
   return true;
}

static bool GITWnameValidationNew(const COLvar& Data, COLstring* pError) {
   COL_FUNCTION(GITWnameValidationNew);
   auto Map = Data.map();
   for(auto i = Map.cbegin(); i != Map.cend(); i++) {
      bool Library   = i->second["type"].asString() == GITUrepoTypeString(GIT_LIBRARY);
      bool Component = i->second["type"].asString() == GITUrepoTypeString(GIT_COMPONENT);
      if(Library && !GITUvalidLibraryName(i->second["name"], pError)) {
         COL_TRC(*pError);
         return false;
      }
      if(Component && !GITUvalidComponentName(i->second["name"], pError)) {
         COL_TRC(*pError);
         return false;
      }
   }
   return true;
}

static bool GITWvalidComponentAndLibraryName(const COLvar& Content, COLstring* pError) {
   COL_FUNCTION(GITWvalidComponentAndLibraryName);
   COL_VAR(Content);
   // OLD way
   if(Content.exists("template")) {
      return GITWnameValidationRunner(Content["template"], GITUvalidComponentName, pError);
   }
   if(Content.exists("library")) { return GITWnameValidationRunner(Content["library"], GITUvalidLibraryName, pError); }
   // NEW way
   if(Content.exists("collection")) { return GITWnameValidationNew(Content["collection"], pError); }
   return true;
}

// TODO refactor this function
static void GITWcollectionSaveImpl(const COLvar& Content, const COLstring& Username, const COLstring& GitUrl,
                                   const COLaddress& Address) {
   COL_FUNCTION(GITWcollectionSaveImpl);
   const COLstring ScratchDir = GITuserScratchDir(Username, GitUrl);
   COL_VAR3(GitUrl, ScratchDir, Content);
   COLstring Out, Error;
   int       ExitCode = -1;
   if(!GITUpermissionWrite(Username, GitUrl, ScratchDir, &ExitCode, &Error)) {
      COL_TRC("You do not have write permission. No point in initializing editable repository.");
      FILremoveFullDirNewSafe(ScratchDir, &Error);
      return COLrespondError(Address,
                             "You do not have write permission. No point in initializing editable repository.");
   }
   if(FILfileExists(ScratchDir)) {
      COL_TRC("Removing directory so that we can make a new one and know that there are no conflicts when we push");
      if(!FILremoveFullDirNewSafe(ScratchDir, &Error)) {
         return COLrespondError(Address, "Unable to remove directory: " + ScratchDir + ". " + Error);
      }
      COL_TRC("scratch dir removed.");
   }
   bool UseSSH = true;
   if(!GITinitializeCachedRepo(Username, GitUrl, "", ScratchDir, UseSSH, &Out, &Error)) {
      COL_TRC("Unable to initialize repository.");
      FILremoveFullDirNewSafe(ScratchDir, &Error);
      return COLrespondError(Address, "Unable to initialize repository: " + GitUrl + ". " + Error);
   }
   Error.clear();

   GITUcollection Collection;
   COLstring      SshUrl;
   if(!GITUsshUrl(GitUrl, &SshUrl)) {
      FILremoveFullDirNewSafe(ScratchDir, &Error);
      return COLrespondError(Address, "Failed to parse git url: " + GitUrl);
   }
   if(!GITWvalidComponentAndLibraryName(Content, &Error)) {
      FILremoveFullDirNewSafe(ScratchDir, &Error);
      return COLrespondError(Address, Error);
   }
   GITUcollectionLoad(&Collection, Content, GitUrl, &Error);

   COLvar FileVar;
   GITUcollectionVar(Collection, &FileVar);
   COL_VAR(FileVar);
   COLstring CollectionFileName = ScratchDir + "collection.json";
   //IX-4331 switching to main branch because bitbucket defaults to master
   if(!FILfileExists(CollectionFileName)){
      COL_TRC("New collection, setting branch to main");
      GITbranchCreateAndCheckout(ScratchDir, "main", &Error);
      COL_VAR(&Error);
      }
   FILserializeSaveConfig(FileVar, CollectionFileName);

   if(!GITadd(ScratchDir, CollectionFileName, true, &Error)) {
      COL_TRC("Unable to add collection.json file to git.");
      FILremoveFullDirNewSafe(ScratchDir, &Error);
      return COLrespondError(Address,
                             "Unable to add collection.json file to git: " + CollectionFileName + ". " + Error);
   }
   if(!GITcommit(Username, ScratchDir, "--allow-empty", "Updated the collection.json file using the Iguana editor.",
                 &Error)) {
      COL_TRC("Unable to commit to git.");
      FILremoveFullDirNewSafe(ScratchDir, &Error);
      return COLrespondError(Address, "Unable to commit to git: " + Error);
   }
   if(!GITpush(Username, ScratchDir, "", &Out, &Error)) {
      COL_TRC("Unable to push to git.");
      FILremoveFullDirNewSafe(ScratchDir, &Error);
      return COLrespondError(Address, "Unable to push to git: " + Error);
   }
   if(FILfileExists(ScratchDir)) {
      COL_TRC("Removing directory to clean up our mess");
      FILremoveFullDirNewSafe(ScratchDir, &Error);  // non-fatal, swallow error.
   }
   COLvar  Result;
   GSVlink Link;
   bool    Success = GSVconstructLink(GitUrl, &Link);
   COLvar  Links   = GSVlinkToVar(Link);
   if(Success) {
      Result["links"] = Links;
   } else {
      Result["error"] = "Error creating link for " + GitUrl;
   }
   COLrespondSuccess(Address, Result);
}

// /git/collection/save
void GITWcollectionSave(const COLwebRequest& Request) {
   COL_FUNCTION(GITWcollectionSave);
   COL_VAR(Request.Data);
   const COLstring GitUrl  = COLvarRequiredString(Request.Data, "git");
   const COLvar    Content = COLvarRequiredObject(Request.Data, "content");
   GITWcollectionSaveImpl(Content, Request.User, GitUrl, Request.Address);
}
