// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GINSautoSet
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Monday 16 September 2024 - 09:11AM
// ---------------------------------------------------------------------------
#include <COL/COLclosure.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <GINS/GINSautoSet.h>
#include <GINS/GINSutils.h>
#include <PRM/PRMcallbacks.h>
#include <SCK/SCKloop.h>
#include <SCK/SCKworker.h>
COL_LOG_MODULE;

void GINSautoSetCallback(COLvar Result, COLaddress Address){
   COL_FUNCTION(GINSautoSetCallback);
	Result["success"].asBool() ? COLrespondSuccess(Address) : COLrespondError(Address, Result["error"].asString());
}

static bool GINSintervalCheck(const int Interval, COLstring* pErr) {
	COL_FUNCTION(GINSintervalCheck);
	if(Interval < 0)     { (*pErr) = "Interval cannot be negative"; return false; }
	if(Interval >= 1440) { (*pErr) = "Interval cannot be greater than a day (1440m)"; return false; }
	return true;
}

void GINSautoSetImpl(const COLvar& In, COLvar* pOut){
   COL_FUNCTION(GINSautoSetImpl);
	const COLstring RequestUser = In["request_user"].asString();
	const int Interval			 = In["interval"].asInt();
	COLstring Err;
	if(!GINSintervalCheck(Interval, &Err)) { (*pOut)["error"] = Err; return; }
	COLvar Config;
	const bool Success = GINSloadAndParse(&Config, &Err);
	COL_VAR3(Success, Config, Err);
	(*pOut)["success"] = Success;
	if(!Success) {
		(*pOut)["error"] = Err;
		return;
	}
	Config["interval"] = Interval;
	try {
		const COLstring File = DIRfileConfigInstance();
		FILwriteFile(File, Config.json(1));
	} catch(const COLerror& E) {
		(*pOut)["success"] = false;
		(*pOut)["error"]	 = E.description();
	}
}

// /instance/interval/set
void GINSautoSet(const COLwebRequest& Request, SCKloop* pLoop){
   COL_FUNCTION(GINSautoSet);
	COLvar Data;
	Data["request_user"] = Request.User;
	Data["interval"]		= COLvarRequiredInt(Request.Data, "interval");
	COLstring Error;
	if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::MANAGE_INSTANCE, &Error)){
		return COLrespondError(Request.Address, PRMsetError(ROLid::MANAGE_INSTANCE));
	}
	SCKinvokeWorker(pLoop, GINSautoSetImpl, Data, COLnewClosure1(GINSautoSetCallback, Request.Address));
}

