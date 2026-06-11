//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GINSreinitialize.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   10/10/24 9:54 AM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GINS/GINSinitialize.h>
#include <GINS/GINSreinitialize.h>
#include <SCK/SCKworker.h>
COL_LOG_MODULE;

static void GINSreinitializeCallback(COLvar Result, COLaddress Address) {
	COL_FUNCTION(GINSreinitializeCallback);
	Result["success"].asBool() ? COLrespondSuccess(Address) : COLrespondError(Address, Result["error"]);
}

bool GINSremoveGitModulesFile(COLstring* pError){
	COL_FUNCTION(GINSremoveGitModulesFile);
	COLstring& Error = *pError;
	const COLstring GitModules = DIRconfigFile(".gitmodules");
	try {
		if(FILfileExists(GitModules)) {
			COL_TRC("Removing " + GitModules);
			FILremove(GitModules, true);
		}
		return true;
	} catch(const COLerror& E) {
		Error = E.description();
		return false;
	}
}

static void GINSreinitializeWorker(const COLvar& In, COLvar* pOut) {
	COL_FUNCTION(GINSreinitializeWorker);
	COLstring Error;
	if(!GINSremoveGitModulesFile(&Error)) {
		(*pOut)["success"] = false;
		(*pOut)["error"]	 = Error;
		COL_VAR(pOut->json(1));
		return;
	}
	const COLstring GitDir = DIRconfigFile(".git/");
	if(FILfileExists(GitDir)) {
		COL_TRC("Removing " + GitDir);
		if (!FILremoveFullDirNewSafe(GitDir, &Error)){
			(*pOut)["success"] = false;
			(*pOut)["error"]	 = Error;
			COL_VAR(pOut->json(1));
			return;
		}
	}
	GINSinitialize();
	(*pOut)["success"] = true;
}

void GINSreinitialize(const COLwebRequest& Request, SCKloop* pLoop) {
	COL_FUNCTION(GINSreinitialize);
	SCKinvokeWorker(pLoop, GINSreinitializeWorker, Request.Data,
						 COLnewClosure1(GINSreinitializeCallback, Request.Address));
}
