#ifndef _SSOCLIENTVERIFY_H
#define _SSOCLIENTVERIFY_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SSOclientVerify.h
//
//  Description: Verify the current logged in use -- jwt validation in SSOjwt
//  Also responsible for re-validating a logged-in user and updates session map accordingly
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLmap.h>

class COLwebRequest;
class SCKloop;
class USRuser;
class WEBsession;

void SSOclientVerify(const COLwebRequest& Request, COLmap<COLstring, USRuser>* pUserMap,
               COLmap<COLstring, WEBsession>* pSessionMap, SCKloop* pLoop);

#endif  // end of defensive include
