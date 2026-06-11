// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITpath
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tuesday 14 February 2023 - 08:41AM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GIT/GITpath.h>
#include <GITU/GITUurl.h>
COL_LOG_MODULE;

// TODO - rationalize DIR/CFG/GITpath - Eliot TODO

COLstring GITcacheDir() {
   COL_FUNCTION(GITcacheDir);
   return FILpathAppend(DIRresourcesDir(), "repos/");
}

COLstring GITbranchDir() {
   COL_FUNCTION(GITbranchDir);
   return FILpathAppend(DIRresourcesDir(), "branch_repos/");
}

COLstring GITUurlDirString(const COLstring& GitUrl) {
   COL_FUNCTION(GITUurlDirString);
   GITUurl   Url;
   COLstring Error;
   if(!GITUparseUri(GitUrl, &Url, &Error)) {
      COL_VAR("Not a git url. Using default value");
      return GitUrl + FIL_DIR_SEPARATOR;
   }
   return Url.Host + FIL_DIR_SEPARATOR + Url.Repository + FIL_DIR_SEPARATOR;
}

COLstring GITuserScratchDir(const COLstring& Username, const COLstring& GitUrl) {
   COL_FUNCTION(GITuserScratchDir);
   COLstring Dir = FILpathAppend(DIRresourceUserScratchDir(Username), GITUurlDirString(GitUrl));
   COL_VAR(Dir);
   return Dir;
}

COLstring GITgitTemplateDir(const COLstring& GitUrl, const COLstring& Branch) {
   COL_FUNCTION(GITgitTemplateDir);
   COLstring Dir;
   if(Branch.is_null()) {
      Dir = FILpathAppend(GITcacheDir(), GITUurlDirString(GitUrl));
   } else {
      Dir = FILpathAppend(GITbranchDir(), FILpathAppend(GITUurlDirString(GitUrl), Branch));  // Host/Repo/<branch_name>
   }
   Dir = FILpathSepAdd(Dir);
   COL_VAR2(GitUrl, Dir);
   return Dir;
}

COLstring GITUmoduleDirFull(const COLstring& ComponentId, const COLstring& ModuleName, bool Development) {
   COL_FUNCTION(GITUmoduleDirFull);
   if(Development) { return GITUmoduleDir(ComponentId, ModuleName); }
   return GITrunModuleDir(ComponentId, ModuleName);
}

COLstring GITUmoduleDir(const COLstring& ComponentId, const COLstring& ModuleName) {
   COL_FUNCTION(GITUmoduleDir);
   if(ModuleName.is_null()) { return DIRcomponentDevDir(ComponentId); }
   return FILpathAppend(DIRcomponentDevDir(ComponentId), ModuleName + FIL_DIR_SEPARATOR);
}

COLstring GITrunModuleDir(const COLstring& ComponentId, const COLstring& ModuleName) {
   COL_FUNCTION(GITrunModuleDir);
   if(ModuleName.is_null()) { return DIRcomponentRunDir(ComponentId); }
   return FILpathAppend(DIRcomponentRunDir(ComponentId), ModuleName + FIL_DIR_SEPARATOR);
}

COLstring GITUcollectionFileName(const COLstring& GitUrl) {
   COL_FUNCTION(GITUcollectionFileName);
   COLstring File = FILpathAppend(GITgitTemplateDir(GitUrl), "collection.json");
   COL_VAR2(GitUrl, File);
   return File;
}

COLstring GITUblankTemplateDir() {
   COL_FUNCTION(GITUblankTemplateDir);
   return FILpathAppend(GITcacheDir(), "blank");
}