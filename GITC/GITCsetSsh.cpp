// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITCsetSsh
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Friday 05 April 2024 - 10:59AM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLweb.h>
#include <FIL/FILutils.h>
#include <GITU/GITUconfigGlobal.h>
#include <GITC/GITCdefaultKeyPair.h>
#include <GITC/GITCsetSsh.h>
#include <GITE/GITEbitbucket.h>
#include <GITE/GITEgithub.h>
#include <GITE/GITEgitlab.h>
#include <GSV/GSVconfig.h>
#include <GSV/GSVmap.h>
COL_LOG_MODULE;

static bool GITCregisterSshKey(const COLstring& PublicKey, const GSVconfig& GitConfig, const GSVitem& Item,
										 COLstring* pError) {
	COL_FUNCTION(GITCregisterSshKey);
	COL_TRC("Registering key for " << GitConfig.m_ServerUrl);
	switch(Item.m_Type) {
		case GIT_bitbucket_cloud:
		case GIT_bitbucket_server:
			return GITEbitbucketRegisterSshKey(PublicKey, GitConfig, pError);
		case GIT_gitlab_cloud:
		case GIT_gitlab_server:
			return GITEgitlabRegisterSshKey(PublicKey, GitConfig, pError);
		case GIT_github_cloud:
		case GIT_github_server:
			return GITEgithubRegisterSshKey(PublicKey, GitConfig, pError);
		case GIT_azuredev_cloud:
		case GIT_azuredev_server:
			*pError = "Azure does not support registering an SSH key via api. Please do this manually.";
			return false;
		default:
			// not implemented
			*pError = "not yet implemented for host: " + GitConfig.m_ServerUrl;
			return false;
	}
}

static void GITCsetSshImpl(const COLstring& Username, const COLaddress& Address) {
	COL_FUNCTION(GITCsetSshImpl);
	COL_TRC("Loading in git service configuration");
	GSVconfig Config;
	if(!GSVconfigLoadUser(Username, &Config)) {
		return COLrespondError(Address, "Could not find a Git Cloud Service configuration file. Please configure this first.");
	}
	if(!GSVmapExists(Config.m_ServerUrl)) {
		return COLrespondError(Address, Config.m_ServerUrl + " not found in server map.");
	}
	COL_TRC("Checking git service configuration username and token");
	COLstring GitServiceUser  = Config.m_Username;
	COLstring GitServiceToken = Config.m_Token;
	COL_VAR2(GitServiceUser, GitServiceToken);
	GSVitem Item = GSVmapItem(Config.m_ServerUrl);
	if(GitServiceUser.is_null() && Item.m_Type == GIT_bitbucket_cloud) {
		return COLrespondError(Address, "Missing Git Cloud Username. Please configure this first.");
	}
	if(GitServiceToken.is_null()) {
		return COLrespondError(Address, "Missing Git Cloud Token. Please configure this first.");
	}
	COL_VAR2(GitServiceUser, GitServiceToken);
	COL_TRC("Checking public and private keys");
	COLstring PrivateKey = GITUconfigPrivateKey(Username);
	COLstring PrivateKeyFile = GITUstripPathsQuotes(PrivateKey);
   // TODO - IX-3403
	COLstring PublicKey	= GITUconfigPublicKey(PrivateKeyFile);
	COL_VAR(PublicKey);
	COLstring ErrorMessage;
	if(!FILfileExists(PrivateKeyFile)) {
		ErrorMessage = "Could not find private key at the configured location: " + PrivateKeyFile;
	}
	if(!FILfileExists(PublicKey)) {
		ErrorMessage = "Could not find public key at the configured location: " + PublicKey;
	}
	if(!ErrorMessage.is_null()) { return COLrespondError(Address, ErrorMessage); }
	COLstring PublicKeyContent;
	try {
		FILreadFile(PublicKey, &PublicKeyContent);
	} catch(const COLerror& Error) { return COLrespondError(Address, Error.description()); }
	COL_TRC("Running register key request");
	COLstring Error;
	if(!GITCregisterSshKey(PublicKeyContent, Config, Item, &Error)) {
		COLrespondError(Address, Error);
	} else {
		COLrespondSuccess(Address);
	}
}

// /git/ssh/set"
void GITCsetSsh(const COLwebRequest& Request) {
	COL_FUNCTION(GITCsetSsh);
	GITCsetSshImpl(Request.User, Request.Address);
}