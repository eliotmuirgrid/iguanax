// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GINSauto
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
#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <FIL/FILutils.h>
#include <GINS/GINSauto.h>
#include <GINS/GINSutils.h>
#include <SCK/SCKloop.h>
#include <SCK/SCKworker.h>
COL_LOG_MODULE;

void GINSautoCallback(COLvar Result, COLaddress Address){
   COL_FUNCTION(GINSautoCallback);
	COL_VAR(Result);
	Result["success"].asBool() ? COLrespondSuccess(Address, Result) : COLrespondError(Address, Result["error"]);
}

void GINSautoImpl(const COLvar& In, COLvar* pOut) {
	COL_FUNCTION(GINSautoImpl);
	COLvar Config;
	COLstring Err;
	const bool Success = GINSloadAndParse(&Config, &Err);
	COL_VAR3(Success, Config, Err);
	(*pOut)["success"] = Success;
	if(!Success) {
		(*pOut)["error"] = Err;
		return;
	}
	(*pOut)["interval"] = Config.exists("interval") ? Config["interval"].asInt() : 0;
}

void GINSauto(const COLwebRequest& Request, SCKloop* pLoop){
   COL_FUNCTION(GINSauto);
	SCKinvokeWorker(pLoop, GINSautoImpl, Request.Data, COLnewClosure1(GINSautoCallback, Request.Address));
}