#ifndef _SSOSAMLREVALIDATE_H
#define _SSOSAMLREVALIDATE_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SSOsamlRevalidate.h
//
//  Description: SAML revalidation to refresh user roles from IdP
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLmap.h>

class COLwebRequest;
class SCKloop;
class WEBsession;

void SSOsamlRevalidate(const COLwebRequest& Request, COLmap<COLstring, WEBsession>* pSessionMap, SCKloop* pLoop);

#endif  // end of defensive include
