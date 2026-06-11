#ifndef _SSOSAMLPOSTHANDLER_H
#define _SSOSAMLPOSTHANDLER_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SSOsamlPostHandler.h
//
//  Description: Handles SAML POST authentication requests, validates credentials, and manages user sessions.
//               This happens after a user logs in through saml sso where the idp send a post to this specific endpoint
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLmap.h>
#include <COL/COLstring.h>

class COLwebRequest;
class SCKloop;
class USRuser;
class WEBrequestData;
class WEBsession;

void SSOsamlPostHandler(const WEBrequestData& RequestData, COLmap<COLstring, USRuser>* pUserMap,
                           COLmap<COLstring, WEBsession>* pSessionMap, SCKloop* pLoop);


#endif  // end of defensive include
