#ifndef __SSOMICROSOFT_H__
#define __SSOMICROSOFT_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SSOmicrosoft.h
//
//  Description: Microsoft SSO role management interface
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLarray.h>

class USRuser;

void      SSOmicrosoftFetchRoles(const COLstring& Token);
COLstring SSOmicrosoftGetRoleName(const COLstring& RoleId);
void      SSOmicrosoftResolveRoleIds(const COLstring& Token, const COLvar& Wids, COLarray<COLstring>& Roles);
bool      SSOmicrosoftExtractUserInfo(const COLstring& Credential, const COLstring& Token, USRuser& User, COLstring& Error);

#endif // end of defensive include
