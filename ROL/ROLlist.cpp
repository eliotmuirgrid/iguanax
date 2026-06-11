//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: ROLlist.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   25/01/24 3:17 PM
//  ---------------------------------------------------------------------------
#include <ROL/ROLlist.h>
#include <ROL/ROLrole.h>
#include <ROL/ROLserialize.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void ROLlist(const COLwebRequest& Request, ROLmap* pRoleMap) {
   COL_FUNCTION(ROLlist);
   COLvar MapVar;
   MapVar.setMapType();
   ROLempty(*pRoleMap) ? COLrespondSuccess(Request.Address, MapVar) : COLrespondSuccess(Request.Address, ROLmapToVar(*pRoleMap));
}