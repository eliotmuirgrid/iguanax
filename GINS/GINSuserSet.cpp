// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GINSuserSet
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
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <GINS/GINSuserSet.h>
#include <GINS/GINSutils.h>
#include <SCK/SCKloop.h>
#include <SCK/SCKworker.h>
#include <PRM/PRMcallbacks.h>
COL_LOG_MODULE;

void GINSuserSetCallback(COLvar Result, COLaddress Address){
   COL_FUNCTION(GINSuserSetCallback);
	COL_VAR(Result);
	Result["success"].asBool() ? COLrespondSuccess(Address) : COLrespondError(Address, Result["error"].asString());
}

void GINSuserSetWorker(const COLvar& In, COLvar* pOut) {
	COL_FUNCTION(GINSuserSetWorker);
	const COLstring RequestUser = In["request_user"];
	const COLstring User			 = In["user"];
	COLvar Config;
	COLstring Err;
	const bool Success = GINSloadAndParse(&Config, &Err);
	COL_VAR3(Success, Config, Err);
	(*pOut)["success"] = Success;
	if(!Success) {
		(*pOut)["error"] = Err;
		return;
	}
	Config["user"] = User;
	try {
		const COLstring File = DIRfileConfigInstance();
		FILwriteFile(File, Config.json(1));
	} catch(const COLerror& E) {
		(*pOut)["success"] = false;
		(*pOut)["error"]	 = E.description();
	}
}

// /instance/user/set
void GINSuserSet(const COLwebRequest& Request, SCKloop* pLoop){
   COL_FUNCTION(GINSuserSet);
	COLvar Data;
	Data["request_user"] = Request.User;
	Data["user"]			= COLvarRequiredString(Request.Data, "user");
	COLstring Error;
	if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::MANAGE_INSTANCE, &Error)){
	   return COLrespondError(Request.Address, PRMsetError(ROLid::MANAGE_INSTANCE));
	}
	SCKinvokeWorker(pLoop, GINSuserSetWorker, Data, COLnewClosure1(GINSuserSetCallback, Request.Address));
}
