//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: ROLaddRole.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   25/01/24 1:51 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <DIR/DIRpath.h>
#include <HASH/HASHutils.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLaddRole.h>
#include <ROL/ROLsave.h>
#include <ROL/ROLid.h>
#include <ROL/ROLrole.h>
#include <ROL/ROLserialize.h>
COL_LOG_MODULE;

static void ROLaddRoleCallback(COLvar Result, ROLmap* pRoleMap, COLstring RoleName, COLaddress Address) {
   COL_FUNCTION(ROLaddRoleCallback);
	COL_VAR(Result);
   if(!Result["success"].asBool()) { return COLrespondError(Address, Result["error"].asString()); }
   ROLadd(pRoleMap, RoleName, ROLrole());
   return COLrespondSuccess(Address, RoleName);
}

void ROLaddRoleImpl(const COLstring& User, ROLmap* pRoleMap, const COLstring& RoleName, const COLstring& File, SCKloop* pLoop,
                    COLauto<COLclosure1<COLvar>> pCallback) {
   COL_FUNCTION(ROLaddRoleImpl);
	COLvar Data		= ROLmapToVar(*pRoleMap);
	Data[RoleName] = ROLroleToVar(ROLrole());
   COL_VAR(Data.json(1));
   ROLsave(User, Data, File, pLoop, pCallback);
}

// /role/add
void ROLaddRole(const COLwebRequest& Request, ROLmap* pRoleMap, SCKloop* pLoop) {
	COL_FUNCTION(ROLaddRole);
	COLstring Error;
	if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::EDIT_ROLES, &Error)) {
		return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_ROLES));
	}
	const COLstring RoleFile = DIRfileConfigRoles();
	const COLstring RoleName = COLvarRequiredString(Request.Data, "name");
	if(!HASHisValid(RoleName))        { return COLrespondError(Request.Address, HASHerror()); }
	if(ROLexists(pRoleMap, RoleName)) { return COLrespondError(Request.Address, RoleName + " already exists"); }
	COLauto<COLclosure1<COLvar>> pCallback = COLnewClosure1(ROLaddRoleCallback, pRoleMap, RoleName, Request.Address);
	ROLaddRoleImpl(Request.User, pRoleMap, RoleName, RoleFile, pLoop, pCallback);
}