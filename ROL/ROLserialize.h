#ifndef __ROLSERIALIZE_H__
#define __ROLSERIALIZE_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: ROLserialize.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   29/01/24 4:01 PM
//  ---------------------------------------------------------------------------
class COLstring;
class COLvar;
class ROLrole;
class ROLmap;

bool ROLvarToRole            (const COLvar& RoleVar, ROLrole* pRole);
bool ROLvarToMap             (const COLvar& RoleMapVar, ROLmap* pRoleMap);
COLvar ROLmapToVar           (const ROLmap& RoleMap);
COLvar ROLroleToVar          (const ROLrole& Role);
COLvar ROLroleToVarIguanaOnly(const ROLrole& Role);

#endif // end of defensive include
