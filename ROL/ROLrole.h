#ifndef __ROLROLE_H__
#define __ROLROLE_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: ROLrole.h
//
//  Description: Interface for roles in IguanaX
//
//  Author: Matthew Sobkowski
//  Date:   24/01/24 2:52 PM
//  ---------------------------------------------------------------------------
#include <ROL/ROLid.h>

bool ROLadd       (ROLmap* pRoleMap, const COLstring& RoleName, const ROLrole& Role);
bool ROLedit      (ROLmap* pRoleMap, const COLstring& RoleName, const ROLrole& Role);
bool ROLdelete    (ROLmap* pRoleMap, const COLstring& RoleName);
bool ROLexists    (ROLmap* pRoleMap, const COLstring& RoleName);
bool ROLcheck     (ROLmap* pRoleMap, const COLstring& RoleName, const ROLid& RoleId);
bool ROLempty     (const ROLmap& RoleMap);
ROLrole ROLgetRole(const ROLmap& RoleMap, const COLstring& RoleName);

#endif // end of defensive include
