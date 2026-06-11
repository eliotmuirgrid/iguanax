//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: ROLqueryRole.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   26/01/24 10:51 AM
//  ---------------------------------------------------------------------------
#include <ROL/ROLqueryRole.h>
#include <ROL/ROLrole.h>
#include <ROL/ROLserialize.h>

#include <COL/COLvarUtils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void ROLqueryRole(const COLwebRequest& Request, ROLmap* pRoleMap) {
   COL_FUNCTION(ROLqueryRole);
   const COLstring RoleName = COLvarRequiredString(Request.Data, "name");
   if(!ROLexists(pRoleMap, RoleName)) { return COLrespondError(Request.Address, RoleName + " does not exist"); }
   const COLvar RoleVar = ROLroleToVar(ROLgetRole(*pRoleMap, RoleName));
   COLrespondSuccess(Request.Address, RoleVar);
}