// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GINSuser
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Monday 16 September 2024 - 07:57AM
// ---------------------------------------------------------------------------
#include <COL/COLclosure.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <FIL/FILutils.h>
#include <GINS/GINSuser.h>
#include <GINS/GINSutils.h>
#include <SCK/SCKloop.h>
#include <SCK/SCKworker.h>
#include <USR/USRuser.h>
COL_LOG_MODULE;

void GINSuserCallback(COLvar Result, COLmap<COLstring, USRuser>* pUserMap, COLaddress Address) {
	COL_FUNCTION(GINSuserCallback);
	COL_VAR(Result);
	Result["exists"] = false;
	if(!Result["success"].asBool()) { return COLrespondError(Address, Result["error"]); }
	Result["exists"] = Result["user"].isNull() ? false : pUserMap->count(Result["user"]) > 0;
	COLrespondSuccess(Address, Result);
}

void GINSuserWorker(const COLvar& In, COLvar* pOut) {
	COL_FUNCTION(GINSuserWorker);
	COLvar Config;
	COLstring Err;
	const bool Success = GINSloadAndParse(&Config, &Err);
	COL_VAR3(Success, Config, Err);
	(*pOut)["success"] = Success;
	if(!Success) {
		(*pOut)["error"] = Err;
		return;
	}
	(*pOut)["user"] = Config["user"];
}

// /instance/user
void GINSuser(const COLwebRequest& Request, SCKloop* pLoop, COLmap<COLstring, USRuser>* pUserMap) {
   COL_FUNCTION(GINSuser);
	SCKinvokeWorker(pLoop, GINSuserWorker, Request.Data, COLnewClosure1(GINSuserCallback, pUserMap, Request.Address));
}
