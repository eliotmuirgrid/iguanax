//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GITUcommitPage.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   24/09/24 11:54 AM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GITU/GITUcommitPag.h>
#include <GIT/GITexecute.h>
#include <GSV/GSVlink.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
COL_LOG_MODULE;

static void GITUcommitPagRepo(const COLstring& Dir, COLvar* pOut) {
	COL_FUNCTION(GITUcommitPagRepo);
	const COLstring Command = "git config --get remote.origin.url";
	COLstring Out, Err;
	const int ExitCode = GITexecuteString(Dir, Command, &Out, &Err);
	COL_VAR3(ExitCode, Out, Err);
	if(ExitCode != 0) {
		(*pOut)["url_error"] = Err;
	} else {
		GSVlink Link;
		const bool Success = GSVconstructLink(Out, &Link);
		if(Success) { (*pOut)["url"] = Link.m_Web + "/commits/"; }
		else        { (*pOut)["url_error"] = "Failed to parse repo: " + Out; }
	}
}

static void GITUcommitPagCount(const COLstring& Dir, const int Skip, COLvar* pOut) {
	COL_FUNCTION(GITUcommitPagCount);
	const COLstring Command = "git rev-list --count HEAD";
	COLstring Out, Err;
	const int ExitCode = GITexecuteString(Dir, Command, &Out, &Err);
	COL_VAR3(ExitCode, Out, Err);
	(*pOut)["total"] = ExitCode == 0 ? atoi(Out.c_str()) : 0;
	(*pOut)["index"] = (*pOut)["commits"].size() + Skip;
}

static void GITUcommitPagParse(const COLstring& Output, COLvar* pOut) {
	COL_FUNCTION(GITUcommitPagParse);
	if(Output.is_null()) { return; }
	COLstring Log, Hash, Message;
	COLstring Rest = Output + "\n";
	(*pOut)["commits"].setArrayType();
	while(Rest.split(Log, Rest, "\n")) {
		Log.split(Hash, Message, " ");
		COL_VAR3(Log, Hash, Message);
		COLvar LogData;
		LogData["hash"]	 = Hash;
		LogData["message"] = Message;
		(*pOut)["commits"].push_back(LogData);
	}
}

static bool GITUcommitPagImpl(const COLstring& Dir, const int Pag, const int Skip, COLvar* pOut) {
	COL_FUNCTION(GITUcommitPagImpl);
	const COLstring PagString	= COLint32ToString(Pag);
	const COLstring SkipString = COLint32ToString(Skip);
	const COLstring Command =
		 "git log --skip=" + SkipString + " --max-count=" + PagString + " --pretty=format:\"%h %s\"";
	COLstring Out, Err;
	const int ExitCode = GITexecuteString(Dir, Command, &Out, &Err);
	COL_VAR3(ExitCode, Out, Err);
	if(ExitCode == 0) {
		GITUcommitPagParse(Out, pOut);
		return true;
	}
	(*pOut)["error"] = Err;
	return false;
}

void GITUcommitPag(const COLwebRequest& Request) {
	COL_FUNCTION(GITUcommitPag);
	COL_VAR(Request.Data.json(1));
	const int Pag		  = COLvarGet(Request.Data, "pag", 10);
	const int Skip		  = COLvarGet(Request.Data, "skip", 0);
	const COLstring Dir = FILpathAppend(FILworkingDir(), "config/");
	COLstring Error;
	if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::MANAGE_INSTANCE, &Error)) {
		return COLrespondError(Request.Address, PRMsetError(ROLid::MANAGE_INSTANCE));
	}
	COLvar Result;
	const bool Success = GITUcommitPagImpl(Dir, Pag, Skip, &Result);
	if(Success) {
		GITUcommitPagCount(Dir, Skip, &Result);
		GITUcommitPagRepo(Dir, &Result);
		COLrespondSuccess(Request.Address, Result);
	} else {
		COLrespondError(Request.Address, Result["error"]);
	}
}