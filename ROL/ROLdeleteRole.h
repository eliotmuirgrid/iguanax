#ifndef __ROLDELETEROLE_H__
#define __ROLDELETEROLE_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: ROLdeleteRole.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   25/01/24 1:51 PM
//  ---------------------------------------------------------------------------
#include <COL/COLweb.h>

class ROLmap;
class SCKloop;

void ROLdeleteRoleImpl(const COLstring& User, ROLmap* pRoleMap, const COLstring& RoleName, const COLstring& File, SCKloop* pLoop,
                       COLauto<COLclosure1<COLvar>> pCallback);
void ROLdeleteRole(const COLwebRequest& Request, ROLmap*  pRoleMap, SCKloop* pLoop);

#endif // end of defensive include
