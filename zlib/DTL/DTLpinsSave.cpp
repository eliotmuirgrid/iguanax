//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: DTLpinsSave.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   16/07/24 3:29 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvarUtils.h>
#include <DIR/DIRpath.h>
#include <DTL/DTLpinsLoad.h>
#include <DTL/DTLpinsSave.h>
#include <FIL/FILutils.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

bool DTLpinsSaveFile(const COLstring& User, const DTLpinsData& Data, COLstring* pError){
	COL_FUNCTION(DTLpinsSaveFile);
	const COLstring UserInterfaces = DIRconfigUserDir(User) + "pins.json";
	COLvar Result;
	DTLpinsToVar(Data, &Result);
	return FILsave(UserInterfaces, Result.json(1), pError);
}

static void DTLpinsSaveImpl(COLstring User, COLvar Pins, CFGmap* pMap, COLaddress Address) {
	COL_FUNCTION(DTLpinsSaveImpl);
	DTLpinsData Data;
	COLstring Error;
	if(!DTLpinsParse(Pins, pMap, &Data, &Error)) { return COLrespondError(Address, Error); };
	if(DTLpinsSaveFile(User, Data, &Error))      { return COLrespondSuccess(Address); }
	return COLrespondError(Address, Error);
}


// /pins/save
void DTLpinsSave(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop) {
	COL_FUNCTION(DTLpinsSave);
	const COLvar& Pins = COLvarRequiredObject(Request.Data, "pins");
	COLstring User = Request.User;
	pLoop->threadPool()->scheduleTask(COLnewClosure0(&DTLpinsSaveImpl, User, Pins, pMap, Request.Address));
}