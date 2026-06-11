//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LOGVapi.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   23/04/25 10:49 AM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <LOGV/LOGVapi.h>
#include <LOGV/LOGVannotation.h>
#include <LOGV/LOGVannotationWin.h>
COL_LOG_MODULE;

void LOGVapi(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap) {
   ApiMap.add("/log/viewer/annotation", &LOGVannotation);
   ApiMap.add("/log/viewer/annotation/window", &LOGVannotationWin);

}