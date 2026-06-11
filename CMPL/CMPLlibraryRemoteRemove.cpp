//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: CMPLlibraryRemoteRemove.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   27/03/25 10:27 AM
//  ---------------------------------------------------------------------------
#include <CFG/CFGconfig.h>
#include <CFG/CFGconfigApi.h>
#include <CFG/CFGmapApi.h>
#include <CFG/CFGmap.h>
#include <CMPC/CMPCutils.h>
#include <CMPL/CMPLlibraryRemoteRemove.h>
#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GITU/GITUremoveRemote.h>
#include <GITU/GITUutils.h>
#include <INI/INIdata.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

void CMPLlibraryRemoveUpstreamHelper(const COLstring& Component, const COLstring& LibraryName) {
   COL_FUNCTION(CMPLlibraryRemoveUpstreamHelper);
   const COLstring ComponentDir = DIRcomponentDevDir(Component);
   const COLstring LibraryDir   = FILpathAppend(DIRcomponentDevDir(Component), LibraryName);
   const COLstring OldRemote    = GITUremoveRemote(LibraryDir);
   // Add to the .gitmodules file
   INIdata   GitModulesData;
   COLstring GitModules = ComponentDir + ".gitmodules";
   GITUreadGitFile(GitModules, &GitModulesData);
   INIdataKeyRemove(&GitModulesData, "submodule", LibraryName, "url");
   GITUsaveGitFile(GitModules, GitModulesData);
   // Add to the .git/config file
   INIdata   ConfigData;
   COLstring FileName = ComponentDir + ".git";
   FILaddPathSeparator(FileName);
   FileName += "config";
   COL_VAR2(Component, FileName);
   GITUreadGitFile(FileName, &ConfigData);
   INIdataKeyRemove(&ConfigData, "submodule", LibraryName, "url");
   GITUsaveGitFile(FileName, ConfigData);
}

static void CMPLlibraryRemoteRemoveImpl(COLstring Component, COLstring LibraryName, COLaddress Address, SCKloop* pLoop,
                                        COLclosure0* pCallback) {
   COL_FUNCTION(CMPLlibraryRemoteRemoveImpl);
   CMPLlibraryRemoveUpstreamHelper(Component, LibraryName);
   COLrespondSuccess(Address);
   pLoop->post(pCallback);
}

// /component/library/remote/remove
void CMPLlibraryRemoteRemove(const COLwebRequest& Request, SCKloop* pLoop) {
   COL_FUNCTION(CMPLlibraryRemoteRemove);
   const COLvar& Data        = Request.Data;
   COLstring ComponentId, Error;
   COLstring     LibraryName = COLvarRequiredString(Data, "name");
   bool          Development = COLvarRequiredBool(Data, "development");
   if(!Development) { return COLrespondError(Request.Address, "Cannot modify file structure of a run repository."); }
   if (!CFGcomponentId(Data, &ComponentId, &Error)){
      return COLrespondError(Request.Address, Error);
   }
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, ComponentId, ROLid::EDIT_SCRIPT, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_SCRIPT));
   }
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&CMPLlibraryRemoteRemoveImpl, ComponentId, LibraryName,
                                                    Request.Address, pLoop,
                                                    CMPCcallback("/component/library/remote/remove")));
}
