//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: ROLdeleteRole.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   25/01/24 1:51 PM
//  ---------------------------------------------------------------------------
#include <ROL/ROLdeleteRole.h>
#include <ROL/ROLrole.h>
#include <ROL/ROLsave.h>
#include <ROL/ROLserialize.h>

#include <DIR/DIRpath.h>

#include <PRM/PRMcallbacks.h>

#include <ROL/ROLid.h>

#include <COL/COLvarUtils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void ROLdeleteRoleCallback(COLvar Result, ROLmap* pRoleMap, COLstring RoleName, COLaddress Address) {
   COL_FUNCTION(ROLdeleteRoleCallback);
   COL_VAR(Result);
   if(!Result["success"].asBool()) { return COLrespondError(Address, Result["error"].asString()); }
   ROLdelete(pRoleMap, RoleName);
   return COLrespondSuccess(Address);
}

void ROLdeleteRoleImpl(const COLstring& User, ROLmap* pRoleMap, const COLstring& RoleName, const COLstring& File, SCKloop* pLoop,
                       COLauto<COLclosure1<COLvar>> pCallback) {
   COL_FUNCTION(ROLdeleteRoleImpl);
   COLvar MapVar = ROLmapToVar(*pRoleMap);
   MapVar.erase(RoleName);
   COL_VAR(MapVar.json(1));
   ROLsave(User, MapVar, File, pLoop, pCallback);
}

// /role/delete
void ROLdeleteRole(const COLwebRequest& Request, ROLmap* pRoleMap, SCKloop* pLoop) {
   COL_FUNCTION(ROLdeleteRole);
   COLstring Error;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::EDIT_ROLES, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_ROLES));
   }
   const COLstring RoleFile = DIRfileConfigRoles();
   const COLstring RoleName = COLvarRequiredString(Request.Data, "name");
   if(!ROLexists(pRoleMap, RoleName)) { return COLrespondError(Request.Address, RoleName + " does not exist"); }
   COLauto<COLclosure1<COLvar>> pCallback = COLnewClosure1(ROLdeleteRoleCallback, pRoleMap, RoleName, Request.Address);
   ROLdeleteRoleImpl(Request.User, pRoleMap, RoleName, RoleFile, pLoop, pCallback);
}