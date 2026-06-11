//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LOGRapi.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   16/07/25 11:12 AM
//  ---------------------------------------------------------------------------
#include <LOGR/LOGRapi.h>
#include <LOGR/LOGRsetDir.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void LOGRapi(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap) {
   COL_FUNCTION(LOGRapi);
   ApiMap.add("/log/directory/set", &LOGRsetDir);

}