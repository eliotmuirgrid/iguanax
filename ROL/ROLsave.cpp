//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: ROLsave.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   25/01/24 2:27 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <FIL/FILutils.h>
#include <ROL/ROLsave.h>
#include <SCK/SCKworker.h>
COL_LOG_MODULE;

static void ROLsaveWorker(const COLvar& In, COLvar* pOut) {
	COL_FUNCTION(ROLsaveWorker);
	COLstring Error;
	const bool Success = FILsave(In["file"].asString(), In["content"], &Error);
	(*pOut)["success"] = Success;
	if(!Success) { (*pOut)["error"] = Error; }
}

void ROLsave(const COLstring& User, const COLvar& RoleMapVar, const COLstring& File, SCKloop* pLoop,
							  COLauto<COLclosure1<COLvar>> pCallback) {
   COL_FUNCTION(ROLsave);
   const COLstring Roles = RoleMapVar.json(1);
   try {
   	COLvar Data;
		Data["user"]	 = User;
		Data["file"]	 = File;
		Data["content"] = Roles;
   	SCKinvokeWorker(pLoop, ROLsaveWorker, Data, pCallback);
   } catch (const COLerror& E) {
      COL_ERR(E.description());
   	COLvar Result;
   	Result["success"] = false;
   	Result["error"] = E.description();
      pCallback->run(Result);
   }
}