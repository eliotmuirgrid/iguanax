#ifndef PRMCALLBACKS_H
#define PRMCALLBACKS_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: PRMcallbacks.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   31/01/24 12:05 PM
//  ---------------------------------------------------------------------------
#include <COL/COLmap.h>
#include <ROL/ROLid.h>
class COLstring;
class SCKloop;
class USRuser;
struct CFGmap;

bool PRMcheckRolePermissions(bool FromTranslator, const COLstring& Agent, const COLstring& Target, ROLrole* pAgentRole, COLstring* pError);
bool PRMcheckRoleAccess(bool FromTranslator, const COLstring& Agent, const COLstring& Target, const ROLid& Id, COLstring* pError);
void PRMsetRoleCallbacks(SCKloop* pLoop, const COLmap<COLstring, USRuser>* pUserMap, ROLmap* pRoleMap, CFGmap* pComponentMap);

COLstring PRMsetError(const ROLid& Id);

#endif // end of defensive include
