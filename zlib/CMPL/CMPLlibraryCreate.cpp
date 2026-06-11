//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: CMPLlibraryCreate.cpp
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
#include <CMPL/CMPLlibraryCreate.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GIT/GITadd.h>
#include <GIT/GITinit.h>
#include <GITU/GITUutils.h>
#include <INI/INIdata.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

static void CMPLaddToSubmodulesFileCreate(const COLstring& ComponentId, const COLstring& LibraryName){
   COL_FUNCTION(CMPLaddToSubmodulesFileCreate);
   INIdata GitModulesData;
   COLstring GitModules = DIRcomponentDevDir(ComponentId) + ".gitmodules";
   GITUreadGitFile(GitModules, &GitModulesData);
   INIdataKeySet(&GitModulesData,"submodule", LibraryName, "path", LibraryName);
   GITUsaveGitFile(GitModules, GitModulesData);
}

static void CMPLaddToConfigFileCreate(const COLstring& ComponentId, const COLstring& LibraryName){
   COL_FUNCTION(CMPLaddToConfigFileCreate);
   INIdata ConfigData;
   COLstring FileName = DIRcomponentDevDir(ComponentId);
   FileName += ".git";
   FILaddPathSeparator(FileName);
   FileName += "config";
   COL_VAR3(ComponentId, LibraryName, FileName);
   GITUreadGitFile(FileName, &ConfigData);
   INIdataKeySet(&ConfigData,"submodule", LibraryName, "active", "true");
   GITUsaveGitFile(FileName, ConfigData);
}

static void CMPLlibraryCreateImpl(COLvar Data, COLaddress Address, SCKloop* pLoop, COLclosure0* pCallback){
   COL_FUNCTION(CMPLlibraryCreateImpl);
	const COLstring ComponentId = Data["component"];
	const COLstring Username	 = Data["username"];
	const COLstring LibraryName = Data["name"];
	const COLstring LibDir		 = FILpathAppend(DIRcomponentDevDir(ComponentId), LibraryName);
	const COLstring CompDir		 = DIRcomponentDevDir(ComponentId);
	COLstring Error;
   CMPLaddToSubmodulesFileCreate(ComponentId, LibraryName);
   CMPLaddToConfigFileCreate    (ComponentId, LibraryName);
   COLstring LibError = "Could not create library: ";
	if(!GITinit(Username, LibDir, true, &Error)) {
		COLrespondError(Address, LibError + Error);
		return pLoop->post(pCallback);
	}
   COLrespondSuccess(Address);
	pLoop->post(pCallback);
}

// /component/library/create
void CMPLlibraryCreate(const COLwebRequest& Request, SCKloop* pLoop) {
   COL_FUNCTION(CMPLlibraryCreate);
   COL_VAR(Request.Data.json(1));
   COLstring ComponentId, Error;
	bool Development			  = COLvarRequiredBool(Request.Data, "development");
	if(!Development) { return COLrespondError(Request.Address, "Cannot modify file structure of a run repository."); }
   if (!CFGcomponentId(Request.Data, &ComponentId, &Error)){
      return COLrespondError(Request.Address, Error);
   }
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, ComponentId, ROLid::EDIT_SCRIPT, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_SCRIPT));
   }
   if(!FILfileExists(FILpathAppend(DIRcomponentDevDir(ComponentId), ".git"))) {
		return COLrespondError(Request.Address, "Cannot create library in plain component.");
   }
	COLvar Data;
	Data["component"] = ComponentId;
	Data["username"]	= Request.User;
	Data["name"]		= COLvarRequiredString(Request.Data, "name");
   if(!GITUvalidLibraryName(Data["name"], &Error)) { return COLrespondError(Request.Address, Error); }
	pLoop->threadPool()->scheduleTask(
		 COLnewClosure0(&CMPLlibraryCreateImpl, Data, Request.Address, pLoop, CMPCcallback("/component/library/create")));
}