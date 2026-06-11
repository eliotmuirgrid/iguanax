//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: HAapi.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <HA/HAapi.h>
#include <HA/HAget.h>
#include <HA/HAset.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void HAapi(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap) {
   COL_FUNCTION(HAapi);
   ApiMap.add("/ha/set", &HAset);
   ApiMap.add("/ha/get", &HAget);
}