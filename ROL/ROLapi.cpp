//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: ROLapi.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   25/01/24 12:47 PM
//  ---------------------------------------------------------------------------
#include <ROL/ROLapi.h>
#include <ROL/ROLaddRole.h>
#include <ROL/ROLdeleteRole.h>
#include <ROL/ROLeditRole.h>
#include <ROL/ROLlist.h>
#include <ROL/ROLqueryRole.h>
#include <ROL/ROLtest.h>

#include <SCK/SCKloop.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void ROLapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, SCKloop* pLoop, ROLmap* pRoleMap) {
   COL_FUNCTION(ROLapi);
   ApiMap.add("/role/list",   COLnewClosure1(&ROLlist, pRoleMap));
   ApiMap.add("/role/query",  COLnewClosure1(&ROLqueryRole, pRoleMap));
   ApiMap.add("/role/test",   COLnewClosure1(&ROLtest, pRoleMap));
   ApiMap.add("/role/add",    COLnewClosure1(&ROLaddRole, pRoleMap, pLoop));
   ApiMap.add("/role/edit",   COLnewClosure1(&ROLeditRole, pRoleMap, pLoop));
   ApiMap.add("/role/delete", COLnewClosure1(&ROLdeleteRole, pRoleMap, pLoop));
}