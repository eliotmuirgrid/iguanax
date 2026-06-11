//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: ROLeditRole.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   26/01/24 10:52 AM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <DIR/DIRpath.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLeditRole.h>
#include <ROL/ROLsave.h>
#include <ROL/ROLid.h>
#include <ROL/ROLrole.h>
#include <ROL/ROLserialize.h>
COL_LOG_MODULE;

static COLvar ROLeditErrorVar(const COLstring& Message) {
	COLvar ErrorVar;
	ErrorVar["success"] = false;
	ErrorVar["error"]	  = Message;
	return ErrorVar;
}

void ROLeditRoleCallback(COLvar Result, ROLmap* pRoleMap, COLstring RoleName, COLvar RoleVar, COLaddress Address) {
   COL_FUNCTION(ROLeditRoleCallback);
	COL_VAR(Result);
   if(!Result["success"].asBool()) { return COLrespondError(Address, Result["error"].asString()); }
   ROLrole Role;
   ROLvarToRole(RoleVar, &Role);
   ROLedit(pRoleMap, RoleName, Role);
   COLrespondSuccess(Address, RoleName);
}

void ROLeditRoleImpl(const COLstring& User, ROLmap* pRoleMap, const COLstring& RoleName, const COLvar& Role,
							const COLstring& File, SCKloop* pLoop, COLauto<COLclosure1<COLvar>> pCallback) {
   COL_FUNCTION(ROLeditRoleImpl);
	COLvar Result;
   if(!ROLexists(pRoleMap, RoleName)) {
      pCallback->run(ROLeditErrorVar(RoleName + " does not exist"));
      return;
   }
   ROLrole Test;
   if(!ROLvarToRole(Role, &Test)) {
      COL_TRC("Invalid Role var -- " << Role);
   	pCallback->run(ROLeditErrorVar(RoleName + " is invalid"));
      return;
   }
   COLvar MapVar = ROLmapToVar(*pRoleMap);
   MapVar[RoleName] = Role;
   COL_VAR(MapVar.json(1));
   ROLsave(User, MapVar, File, pLoop, pCallback);
}

// /role/edit
void ROLeditRole(const COLwebRequest& Request, ROLmap* pRoleMap, SCKloop* pLoop) {
   COL_FUNCTION(ROLeditRole);
   COLstring Error;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::EDIT_ROLES, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_ROLES));
   }
   const COLstring RoleFile = DIRfileConfigRoles();
   const COLstring RoleName = COLvarRequiredString(Request.Data, "name");
   COLvar          Role     = COLvarRequiredObject(Request.Data, "role");
   if(!Role.isMap()){return COLrespondError(Request.Address,"role parameter must be a map");}
   if(!Role.exists("unsecure")) { Role["unsecure"] = false; }
   COLauto<COLclosure1<COLvar>> pCallback = COLnewClosure1(ROLeditRoleCallback, pRoleMap, RoleName, Role, Request.Address);
   ROLeditRoleImpl(Request.User, pRoleMap, RoleName, Role, RoleFile, pLoop, pCallback);
}
