//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GINIapi.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   21/05/25 11:58 AM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <GINI/GINIapi.h>
#include <GINI/GINIcheckKeyTracking.h>
#include <GINI/GINIsetKeyTracking.h>
COL_LOG_MODULE;

void GINIapi(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap) {
   COL_FUNCTION(GINIapi);
   ApiMap.add("/instance/track_keys", &GINIcheckKeyTracking);
   ApiMap.add("/instance/set_key_tracking", &GINIsetKeyTracking);
}

