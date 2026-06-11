#ifndef _SSOAPI_H
#define _SSOAPI_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SSOapi.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-09-22, 10:44a.m.
//  ---------------------------------------------------------------------------
#include <COL/COLhashmap.h>
#include <COL/COLweb.h>

class SCKloop;
class USRuser;
class WEBsession;

void SSOapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap,
            COLmap<COLstring, USRuser>* pUserMap, COLmap<COLstring, WEBsession>* pSessionMap, SCKloop* pLoop);

#endif  // end of defensive include
