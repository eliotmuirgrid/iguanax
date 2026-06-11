#ifndef __PRMROLECHECK_H__
#define __PRMROLECHECK_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: PRMroleCheck.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   29/01/24 1:02 PM
//  ---------------------------------------------------------------------------
#include <COL/COLmap.h>
#include <ROL/ROLid.h>
class COLstring;
class SCKloop;
class USRuser;
struct CFGmap;

void PRMroleChecker(bool FromTranslator, const COLstring& Agent, const COLstring& Target, ROLrole* pAgentRole,
                    COLstring* pError, const COLmap<COLstring, USRuser>* pUserMap, ROLmap* pRoleMap, CFGmap* pComponentMap);
#endif // end of defensive include
