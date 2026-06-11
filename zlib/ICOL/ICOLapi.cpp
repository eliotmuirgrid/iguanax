//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: ICOLapi.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   20/03/25 1:50 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <ICOL/ICOLapi.h>
#include <ICOL/ICOLcolumns.h>
COL_LOG_MODULE;

void ICOLapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap) {
   COL_FUNCTION(ICOLapi);
   ApiMap.add("/instance/columns", COLnewClosure1(&ICOLcolumns));
}
