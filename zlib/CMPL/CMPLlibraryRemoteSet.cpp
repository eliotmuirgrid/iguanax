//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: CMPLlibraryRemoteSet.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   27/03/25 10:27 AM
//  ---------------------------------------------------------------------------
#include <CFG/CFGconfig.h>
#include <CFG/CFGmapApi.h>
#include <CMPC/CMPCutils.h>
#include <CMPL/CMPLlibraryRemoteSet.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GIT/GITpush.h>
#include <GITU/GITUaddRemote.h>
#include <GITU/GITUurl.h>
#include <GITU/GITUutils.h>
#include <INI/INIdata.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

void CMPLlibraryRemoteSetImpl(COLvar Data, COLaddress Address, SCKloop* pLoop, COLclosure0* pCallback) {
   COL_FUNCTION(CMPLlibraryRemoteSetImpl);
   const COLstring Username     = Data["username"];
   const COLstring Component    = Data["component"];
   const COLstring LibraryName  = Data["name"];
   const COLstring GitUrl       = Data["url"];
   const COLstring ComponentDir = DIRcomponentDevDir(Component);
   const COLstring LibraryDir   = ComponentDir + LibraryName;
   const bool      Push         = Data["push"];
   COLstring       Err;
   COL_VAR2(ComponentDir, GitUrl);
   // Normalize the url to ssh form. Output is host dependent
   if (!FILfileExists(ComponentDir + ".git")) {
      Err = "Cannot set remote for library in a plain component.";
      COLrespondError(Address, Err);
      return pLoop->post(pCallback);
   }
   COLstring Url;
   if(!GITUcleanUrl(GitUrl, &Url)) {
      Err = "Failed to parse url into a valid git format";
      COL_TRC(Err);
      COLrespondError(Address, Err);
      return pLoop->post(pCallback);
   }
   COL_VAR(Url);
   if(!GITUaddRemote(LibraryDir, Url, &Err)) {
      COLrespondError(Address, Err);
      return pLoop->post(pCallback);
   }
   // Add to the .gitmodules file
   INIdata   GitModulesData;
   COLstring GitModules = ComponentDir + ".gitmodules";
   GITUreadGitFile(GitModules, &GitModulesData);
   INIdataKeySet(&GitModulesData, "submodule", LibraryName, "url", Url);
   GITUsaveGitFile(GitModules, GitModulesData);
   // Add to the .git/config file
   INIdata   ConfigData;
   COLstring FileName = ComponentDir + ".git";
   FILaddPathSeparator(FileName);
   FileName += "config";
   COL_VAR2(Component, FileName);
   GITUreadGitFile(FileName, &ConfigData);
   INIdataKeySet(&ConfigData, "submodule", LibraryName, "url", Url);
   COL_VAR2(LibraryName, GitUrl);
   GITUsaveGitFile(FileName, ConfigData);
   COLstring Out;
   if(Push && !GITpush(Username, LibraryDir, "", &Out, &Err)) {
      COL_VAR2(Out, Err);
      COLrespondError(Address, Err);
   } else {
      COLrespondSuccess(Address);
   }
   pLoop->post(pCallback);
}

// "/component/library/set_upstream"
void CMPLlibraryRemoteSet(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop) {
   COL_FUNCTION(CMPLlibraryRemoteSet);
   const COLstring Component   = COLvarRequiredString(Request.Data, "component");
   bool            Development = COLvarRequiredBool(Request.Data, "development");
   if(!Development) {
      return COLrespondError(Request.Address, "Cannot modify upstream of a library inside of a run repository.");
   }
   CFGconfig* pConfig = CFGmapNode(pMap, Component);
   if(!pConfig) { return COLrespondError(Request.Address, Component); }
   COLstring Error;
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, pConfig->CoreConfig.Guid, ROLid::EDIT_SCRIPT, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_SCRIPT));
   }
   COLvar Data;
   Data["username"]  = Request.User;
   Data["component"] = Component;
   Data["name"]      = COLvarRequiredString(Request.Data, "name");
   Data["url"]       = COLvarRequiredString(Request.Data, "git");
   Data["push"]      = COLvarGetBool(Request.Data, "push", false);
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&CMPLlibraryRemoteSetImpl, Data, Request.Address, pLoop,
                                                    CMPCcallback("/component/library/set_upstream")));
}
