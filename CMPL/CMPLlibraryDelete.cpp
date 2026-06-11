//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: CMPLlibraryDelete.cpp
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
#include <CMPL/CMPLlibraryDelete.h>
#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GITU/GITUconfig.h>
#include <GITU/GITUutils.h>
#include <INI/INIdata.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

static void CMPLremoveFromSubmodulesFile(const COLstring& ComponentId, const COLstring& LibraryName) {
   COL_FUNCTION(CMPLremoveFromSubmodulesFile);
   INIdata   GitModulesData;
   COLstring GitModules = DIRcomponentDevDir(ComponentId) + ".gitmodules";
   GITUreadGitFile(GitModules, &GitModulesData);
   GITUconfigSubmoduleClear(&GitModulesData, LibraryName);
   GITUsaveGitFile(GitModules, GitModulesData);
}

static void CMPLremoveFromConfigFile(const COLstring& ComponentId, const COLstring& LibraryName) {
   COL_FUNCTION(CMPLremoveFromConfigFile);
   INIdata   ConfigData;
   COLstring FileName = DIRcomponentDevDir(ComponentId);
   FileName += ".git";
   FILaddPathSeparator(FileName);
   FileName += "config";
   COL_VAR2(ComponentId, FileName);
   GITUreadGitFile(FileName, &ConfigData);
   GITUconfigSubmoduleClear(&ConfigData, LibraryName);
   GITUsaveGitFile(FileName, ConfigData);
}

static void CMPLlibraryDeleteImpl(COLstring Component, COLstring LibraryName, COLaddress Address, SCKloop* pLoop,
                                  COLclosure0* pCallback) {
   COL_FUNCTION(CMPLlibraryDeleteImpl);
   COLstring LibDir = FILpathAppend(DIRcomponentDevDir(Component), LibraryName);
   COL_VAR(LibDir);
   if(!FILfileExists(LibDir)) {
      COLrespondError(Address, "No folder exists with the name: " + LibraryName);
      return pLoop->post(pCallback);
   }
   COLstring Error;
   if(!FILremoveFullDirNewSafe(LibDir, &Error)) {
      COLrespondError(Address, "Failed to remove library directory: " + LibDir + ". " + Error);
      return pLoop->post(pCallback);
   }
   CMPLremoveFromSubmodulesFile(Component, LibraryName);
   CMPLremoveFromConfigFile(Component, LibraryName);
   CMPCremoveRequireFromAllFiles(DIRcomponentDevDir(Component), LibraryName + ".");
   COLrespondSuccess(Address);
   pLoop->post(pCallback);
}

// /component/library/delete
void CMPLlibraryDelete(const COLwebRequest& Request, SCKloop* pLoop) {
   COL_FUNCTION(CMPLlibraryDelete);
   const COLvar& Data = Request.Data;
   COL_VAR(Data.json(1));
   COLstring ComponentId, Error;
   const COLstring LibraryName = COLvarRequiredString(Data, "name");
   bool            Development = COLvarRequiredBool(Data, "development");
   if(!Development) { return COLrespondError(Request.Address, "Cannot modify file structure of a run repository."); }
   if (!CFGcomponentId(Data, &ComponentId, &Error)){
      return COLrespondError(Request.Address, Error);
   }
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, ComponentId, ROLid::EDIT_SCRIPT, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_SCRIPT));
   }
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&CMPLlibraryDeleteImpl, ComponentId, LibraryName,
                                                    Request.Address, pLoop, CMPCcallback("/component/library/delete")));
}