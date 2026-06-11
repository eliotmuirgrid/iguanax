//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GINRapi.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   17/12/24 10:41 AM
//  ---------------------------------------------------------------------------
#include <GINR/GINRapi.h>
#include <GINR/GINRrestart.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void GINRapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap) {
   COL_FUNCTION(GINRapi);
   ApiMap.add("/instance/restart", COLnewClosure1(&GINRrestart));
}
