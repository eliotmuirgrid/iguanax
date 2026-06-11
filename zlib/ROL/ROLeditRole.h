#ifndef __ROLEDIT_H__
#define __ROLEDIT_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: ROLeditRole.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   26/01/24 10:52 AM
//  ---------------------------------------------------------------------------
#include <COL/COLweb.h>

class ROLmap;
class SCKloop;

void ROLeditRoleImpl(const COLstring& User, ROLmap* pRoleMap, const COLstring& RoleName, const COLvar& Role,
							const COLstring& File, SCKloop* pLoop, COLauto<COLclosure1<COLvar>> pCallback);
void ROLeditRole(const COLwebRequest& Request, ROLmap* pRoleMap, SCKloop* pLoop);

#endif // end of defensive include
