//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: CMPLlibraryRename.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   27/03/25 10:27 AM
//  ---------------------------------------------------------------------------
#include <CFG/CFGconfig.h>
#include <CFG/CFGmapApi.h>
#include <CFG/CFGmap.h>
#include <CMPC/CMPCutils.h>
#include <CMPL/CMPLlibraryRename.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GITU/GITUutils.h>
#include <INI/INIdata.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKloop.h>
#include <SRCH/SRCHreplaceAll.h>
COL_LOG_MODULE;

static void CMPLchangeRequires(const COLstring& Dir, const COLstring& LibraryName, const COLstring& Rename) {
   COL_FUNCTION(CMPLchangeRequires);
   const COLstring  OldRequireSingle = "require \'" + LibraryName + ".";
   const COLstring  NewRequireSingle = "require \'" + Rename + ".";
   const COLstring  OldRequireDouble = "require \"" + LibraryName + ".";
   const COLstring  NewRequireDouble = "require \"" + Rename + ".";
   FILdirEnumerator Enum(Dir + FIL_DIR_SEPARATOR + FIL_ANYFILE);
   try {
      COLstring File;
      while(Enum.getFile(File)) {
         const COLstring Path = FILpathAppend(Dir, File);
         COL_VAR(Path);
         if(FILisDirectory(Path)) { CMPLchangeRequires(Path, LibraryName, Rename); }
         if(FILpathExtLast(Path, true) == ".lua") {
            COLstring Content;
            try {
               FILreadFile(Path, &Content);
               Content.replace(OldRequireSingle, NewRequireSingle);
               Content.replace(OldRequireDouble, NewRequireDouble);
               FILwriteFile(Path, Content);
            } catch(const COLerror& E) { COL_ERR(E.description()); }
         }
      }
   } catch(const COLerror& E) { COL_ERR(E.description()); }
}

static void CMPLmodifyModulesFile(const COLstring& ComponentDir, const COLstring& LibraryName,
                                  const COLstring& Rename) {
   COL_FUNCTION(CMPLmodifyModulesFile);
   INIdata         ModulesData;
   const COLstring GitModules = ComponentDir + ".gitmodules";
   GITUreadGitFile(GitModules, &ModulesData);
   const COLstring GitUrl = INIdataKeyGet(ModulesData, "submodule", LibraryName, "url");
   INIdataSectionRemove(&ModulesData, "submodule", LibraryName);
   INIdataKeySet(&ModulesData, "submodule", Rename, "path", Rename);
   INIdataKeySet(&ModulesData, "submodule", Rename, "url", GitUrl);
   GITUsaveGitFile(GitModules, ModulesData);
}

static void CMPLmodifyConfigFile(const COLstring& ComponentDir, const COLstring& LibraryName, const COLstring& Rename) {
   COL_FUNCTION(CMPLmodifyConfigFile);
   INIdata   ConfigData;
   COLstring ConfigFile = ComponentDir + ".git";
   FILaddPathSeparator(ConfigFile);
   ConfigFile += "config";
   GITUreadGitFile(ConfigFile, &ConfigData);
   const COLstring GitUrl = INIdataKeyGet(ConfigData, "submodule", LibraryName, "url");
   INIdataSectionRemove(&ConfigData, "submodule", LibraryName);
   INIdataKeySet(&ConfigData, "submodule", Rename, "url", GitUrl);
   INIdataKeySet(&ConfigData, "submodule", Rename, "active", "true");
   GITUsaveGitFile(ConfigFile, ConfigData);
}

static void CMPLlibraryRenameImpl(COLvar Data, COLaddress Address, SCKloop* pLoop, COLclosure0* pCallback) {
   COL_FUNCTION(CMPLlibraryRenameImpl);
   const COLstring Component      = Data["component"];
   const COLstring OldLibraryName = Data["library"];
   const COLstring NewLibraryName = Data["rename"];
   const COLstring Dir            = DIRcomponentDevDir(Component);
   const COLstring SubmodulePath  = FILpathAppend(Dir, OldLibraryName);
   const COLstring NewPath        = FILpathAppend(Dir, NewLibraryName);
   COL_VAR3(Dir, SubmodulePath, NewPath);
   if(!FILfileExists(SubmodulePath)) {
      COLrespondError(Address, SubmodulePath + " not found");
      return pLoop->post(pCallback);
   }
   if(FILfileExists(NewPath)) {
      COLrespondError(Address, NewPath + " already exists");
      return pLoop->post(pCallback);
   }
   COL_TRC("Renaming " << SubmodulePath << " to " << NewPath);
   FILrenameDirectory(SubmodulePath, NewPath, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX, true);
   CMPLmodifyConfigFile(Dir, OldLibraryName, NewLibraryName);
   CMPLmodifyModulesFile(Dir, OldLibraryName, NewLibraryName);
   COLstring Error;
   COL_TRC("Rename complete -- changing requires");
   CMPLchangeRequires(Dir, OldLibraryName, NewLibraryName);
   COLrespondSuccess(Address);
   pLoop->post(pCallback);
}

// /component/library/rename
void CMPLlibraryRename(const COLwebRequest& Request, SCKloop* pLoop) {
   COL_FUNCTION(CMPLlibraryRename);
   COLstring ComponentId, Error;
   bool Development = COLvarRequiredBool(Request.Data, "development");
   if(!Development) { return COLrespondError(Request.Address, "Cannot modify file structure of a run repository."); }
   if (!CFGcomponentId(Request.Data, &ComponentId, &Error)){
      return COLrespondError(Request.Address, Error);
   }
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, ComponentId, ROLid::EDIT_SCRIPT, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_SCRIPT));
   }
   COLvar Data;
   Data["component"] = ComponentId;
   Data["library"]   = COLvarRequiredString(Request.Data, "library");
   Data["rename"]    = COLvarRequiredString(Request.Data, "rename");
   if(!GITUvalidLibraryName(Data["rename"], &Error)) { return COLrespondError(Request.Address, Error); }
   pLoop->threadPool()->scheduleTask(
       COLnewClosure0(&CMPLlibraryRenameImpl, Data, Request.Address, pLoop, CMPCcallback("/component/library/rename")));
}
