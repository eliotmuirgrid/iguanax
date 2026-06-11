#ifndef __GITEAZUREENDPOINTS_H__
#define __GITEAZUREENDPOINTS_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GITEazureEndpoints.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   26/07/24 2:27 PM
//  ---------------------------------------------------------------------------
class COLstring;
class GSVconfig;
class WEBrequest;

extern const COLstring GITE_AZURE_DEV_URL;
extern const COLstring GITE_AZURE_VSAEX_URL;

void GITEazureSetupRequest(const COLstring& Path, const GSVconfig& Config, WEBrequest* pRequest);

COLstring GITEazureEncodeToken(const COLstring& Token);
COLstring GITEazureTokenEndpoint(const GSVconfig& Config, bool Secure);
COLstring GITEazureUserEndpoint(const GSVconfig& Config, bool Secure);
COLstring GITEazureOrgEndpoint(const COLstring& UserId, const GSVconfig& Config, bool Secure);
COLstring GITEazureProjToOwnerEndpoint(const COLstring& Organization, const GSVconfig& Config, bool Secure);
COLstring GITEazureRepoEndpoint(const COLstring& Owner, const COLstring& Slug, const GSVconfig& Config, bool Secure);
COLstring GITEazureNewRepoEndpoint(const COLstring& Owner, const GSVconfig& Config, bool Secure);

#endif  // end of defensive include
