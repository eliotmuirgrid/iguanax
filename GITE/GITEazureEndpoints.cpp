//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GITEazureEndpoints.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   26/07/24 2:27 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <GITE/GITEazureEndpoints.h>
#include <GITE/GITEutils.h>
#include <GSV/GSVconfig.h>
#include <GSV/GSVlink.h>
#include <SFI/SFIbase64EncodeFilter.h>
#include <WEB/WEBrequest.h>
COL_LOG_MODULE;

const COLstring GITE_AZURE_DEV_URL = "https://dev.azure.com/";
const COLstring GITE_AZURE_VSAEX_URL = "https://vsaex.dev.azure.com/";

void GITEazureSetupRequest(const COLstring& Path, const GSVconfig& Config, WEBrequest* pRequest) {
	COL_FUNCTION(GITEazureSetupRequest);
	pRequest->Url = Path;
	pRequest->Headers["Authorization"] = "Basic " + GITEazureEncodeToken(Config.m_Token);
	pRequest->Headers["Accept"]        = "application/json";
	pRequest->Headers["Content-Type"]  = "application/json";
	pRequest->Timeout = 15000;
	COL_VAR(pRequest->Headers);
}

COLstring GITEazureEncodeToken(const COLstring& Token) { 
	COLstring Out;
	SFIbase64EncodeOneLine(COL_T(":") + Token, &Out);
	COL_VAR(Out);
	return Out;
}

COLstring GITEazureCreateServerUrl(const COLstring& BaseUrl) {
	COL_FUNCTION(GITEazureCreateServerUrl);
	COLstring Url;
	GSVconstructServerLink(BaseUrl, &Url);
	return Url;
}

COLstring GITEazureTokenEndpoint(const GSVconfig& Config, bool Secure) {
	COL_FUNCTION(GITEazureTokenEndpoint);
	COL_VAR(Config.m_Local);
	COL_VAR(Secure);
	if (!Config.m_Local) { return GITE_AZURE_VSAEX_URL + Config.m_Username + "/_apis/memberentitlements?api-version=7.1-preview.2"; }
	return GITEazureCreateServerUrl(Config.m_ServerUrl) + "/_apis/projectCollections?api-version=6.0";
}

COLstring GITEazureUserEndpoint(const GSVconfig& Config, bool Secure) {
	COL_FUNCTION(GITEazureUserEndpoint);
	const COLstring Endpoint = "/_apis/connectionData";
	if (!Config.m_Local) { return GITE_AZURE_VSAEX_URL + Config.m_Username + Endpoint; }
	return GITEazureCreateServerUrl(Config.m_ServerUrl) + Endpoint;
}

COLstring GITEazureOrgEndpoint(const COLstring& UserId, const GSVconfig& Config, bool Secure) {
	COL_FUNCTION(GITEazureOrgEndpoint);
	if(!Config.m_Local) { return "https://app.vssps.visualstudio.com/_apis/accounts?memberId=" + UserId + "&api-version=6.0"; }
	return GITEazureCreateServerUrl(Config.m_ServerUrl) + "/_apis/projectCollections";
}

COLstring GITEazureProjToOwnerEndpoint(const COLstring& Organization, const GSVconfig& Config, bool Secure) {
	COL_FUNCTION(GITEazureProjToOwnerEndpoint);
	if(!Config.m_Local) { return GITE_AZURE_DEV_URL + GITEencodeString(Organization) + "/_apis/projects?api-version=6.0"; }
	return GITEazureCreateServerUrl(Config.m_ServerUrl) + "/" + GITEencodeString(Organization) + "/_apis/projects?api-version=6.0-preview.4";
}

COLstring GITEazureRepoEndpoint(const COLstring& Owner, const COLstring& Slug, const GSVconfig& Config, bool Secure) {
	COL_FUNCTION(GITEazureRepoEndpoint);
	if(!Config.m_Local) { return GITE_AZURE_DEV_URL + GITEencodeString(Owner) + "/_apis/git/repositories/" + Slug + "?api-version=7.1-preview.1"; }
	return GITEazureCreateServerUrl(Config.m_ServerUrl) +  "/" + GITEencodeString(Owner) + "/_apis/git/repositories/" + Slug + "?api-version=7.1-preview.1";
}

COLstring GITEazureNewRepoEndpoint(const COLstring& Owner, const GSVconfig& Config, bool Secure) {
	COL_FUNCTION(GITEazureNewRepoEndpoint);
	if(!Config.m_Local) { return GITE_AZURE_DEV_URL + GITEencodeString(Owner) + "/_apis/git/repositories?api-version=7.1-preview.1"; }
	return GITEazureCreateServerUrl(Config.m_ServerUrl) +  "/" + GITEencodeString(Owner) + "/_apis/git/repositories/?api-version=7.1-preview.1";
}