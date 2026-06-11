//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GINScommitAll.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   26/09/24 10:56 AM
//  ---------------------------------------------------------------------------
#include <CFG/CFGmap.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GINS/GINScommitAll.h>
#include <GINS/GINSutils.h>
#include <GIT/GITexecute.h>
#include <SCK/SCKloop.h>
#include <SCK/SCKworker.h>
COL_LOG_MODULE;

static void GINScommitAllCallback(COLvar Result, COLaddress Address) {
	COL_FUNCTION(GINScommitAllCallback);
	COL_VAR(Result);
	Result["success"].asBool() ? COLrespondSuccess(Address) : COLrespondError(Address, Result["error"]);
}

static void GINcommitAllSubmodules(const COLstring& Dir, const COLstring& Out) {
	COL_FUNCTION(GINcommitAllSubmodules);
	COL_VAR2(Dir, Out);
	if(Out.is_null()) { return; }
	COLstring GitOut, GitErr;
	const COLstring Command = "git submodule foreach --recursive \"git add . && git commit -m 'Updated submodule' || echo 'No changes to commit'\"";
	const int Code = GITexecuteString(Dir, Command, &GitOut, &GitErr);
	COL_VAR4(Command, Code, GitOut, GitErr);
	COLstring Line, Prefix, File;
	COLstring Rest = Out + "\n";
	while(Rest.split(Line, Rest, "\n")) {
		Line = COLstripWhiteSpace(Line);
		Line.split(Prefix, File, " ");
		if(!File.is_null()) {
			GitOut.clear();
			GitErr.clear();
			COL_VAR3(Line, Prefix, File);
			const int Code = GITexecuteString(Dir, "git add " + File, &GitOut, &GitErr);
			COL_VAR4(File, Code, GitOut, GitErr);
		}
	}
}

static void GINScommitAllImpl(const COLvar& In, COLvar* pOut) {
	COL_FUNCTION(GINScommitAllImpl);
	COLstring Out, Err;
	const COLstring Dir = FILpathAppend(FILworkingDir(), "config/");
	bool Success		  = GINSstatusClean(Dir, &Out, &Err);
	if(!Success) { GINcommitAllSubmodules(Dir, Out); }
	Out.clear();
	Err.clear();
	GITexecuteString(Dir, "git add .",&Out, &Err);
	const int Code = GITexecuteString(Dir, "git commit -m \"" + In["message"].asString() + "\"" ,&Out, &Err);
	(*pOut)["success"] = Code == 0;
	if(!Success) { (*pOut)["error"] = Err; }
	COL_VAR3(Success, Out, Err);
}

void GINScommitAll(const COLwebRequest& Request, SCKloop* pLoop) {
	COL_FUNCTION(GINScommitAll);
	COLvar Data;
	Data["user"]	 = Request.User;
	Data["message"] = COLvarRequiredString(Request.Data, "message");
	SCKinvokeWorker(pLoop, GINScommitAllImpl, Data, COLnewClosure1(GINScommitAllCallback, Request.Address));
}