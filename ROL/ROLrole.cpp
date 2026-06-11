//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: ROLrole.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   24/01/24 2:52 PM
//  ---------------------------------------------------------------------------
#include <ROL/ROLrole.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

bool ROLadd(ROLmap* pRoleMap, const COLstring& RoleName, const ROLrole& Role) {
   COL_FUNCTION(ROLadd);
   if(ROLexists(pRoleMap, RoleName)) { return false; }
   pRoleMap->m_Roles[RoleName] = Role;
   return true;
}

bool ROLedit(ROLmap* pRoleMap, const COLstring& RoleName, const ROLrole& Role) {
   COL_FUNCTION(ROLedit);
   if(!ROLexists(pRoleMap, RoleName)) { return false; }
   pRoleMap->m_Roles[RoleName] = Role;
   return true;
}

bool ROLdelete(ROLmap* pRoleMap, const COLstring& RoleName) {
   COL_FUNCTION(ROLdelete);
   if(!ROLexists(pRoleMap, RoleName)) { return false; }
   pRoleMap->m_Roles.erase(RoleName);
   return true;
}

bool ROLexists(ROLmap* pRoleMap, const COLstring& RoleName) {
   COL_FUNCTION(ROLexists);
   return !ROLempty(*pRoleMap) && pRoleMap->m_Roles.count(RoleName) != 0;
}

bool ROLcheck(ROLmap* pRoleMap, const COLstring& RoleName, const ROLid& RoleId) {
   COL_FUNCTION(ROLcheck);
   if(!ROLexists(pRoleMap, RoleName)) { return false; }
   return pRoleMap->m_Roles[RoleName].m_Role[RoleId];
}

bool ROLempty(const ROLmap& RoleMap) {
   COL_FUNCTION(ROLempty);
   return RoleMap.m_Roles.empty();
}

ROLrole ROLgetRole (const ROLmap& RoleMap, const COLstring& RoleName) {
   COL_FUNCTION(ROLgetRole);
   return RoleMap.m_Roles[RoleName];
}