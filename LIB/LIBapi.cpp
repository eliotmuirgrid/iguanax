//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LIBapi.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   09/06/25 2:32 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <LIB/LIBapi.h>
#include <LIB/LIBinfo.h>
#include <LIB/LIBupdate.h>
COL_LOG_MODULE;

void LIBapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap, CFGmap* pMap, SCKloop* pLoop) {
   COL_FUNCTION(LIBapi);
   ApiMap.add("/library/info", COLnewClosure1(&LIBinfo, pMap, pLoop));
   ApiMap.add("/library/update", COLnewClosure1(&LIBupdate, pLoop));

}