// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITCupdateOwners
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Tuesday 09 April 2024 - 12:29PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <GITC/GITCupdateOwners.h>
#include <GITE/GITEazureDevops.h>
#include <GITE/GITEbitbucket.h>
#include <GITE/GITEgithub.h>
#include <GITE/GITEgitlab.h>
#include <GSV/GSVconfig.h>
#include <GSV/GSVmap.h>
COL_LOG_MODULE;

static void GITCupdateOwnersImpl(const COLstring& Username, bool Force, const COLaddress& Address) {
	COL_FUNCTION(GITCupdateOwnersImpl);
	COLvar Result;
	COLstring Error;
   GSVconfig Config;
   GSVconfigLoadUser(Username, &Config);
	COL_VAR(Config.m_ServerUrl);
	bool Success = false;
	if(!GSVmapExists(Config.m_ServerUrl)) { Error = Config.m_ServerUrl + " not found in server list"; }
	else {
		const GSVitem Item = GSVmapItem(Config.m_ServerUrl);
		switch (Item.m_Type){
			case GIT_azuredev_cloud:  
			case GIT_azuredev_server:
				Success = GITEazureUpdateOwners(Username, Force, Config, Item.m_Secure, &Result, &Error);     break;
			case GIT_bitbucket_cloud: 
			case GIT_bitbucket_server:
				Success = GITEbitbucketUpdateOwners(Username, Force, Config, &Result, &Error); break;
			case GIT_gitlab_cloud:    
			case GIT_gitlab_server:
				Success = GITEgitlabUpdateOwners(Username, Force, Config, &Result, &Error);    break;
			case GIT_github_cloud:    
			case GIT_github_server:
				Success = GITEgithubUpdateOwners(Username, Force, Config, &Result, &Error);    break;
			default:
				Success = false;
				Error = "Not yet implemented";;
		}
	}
   if(!Success){
      COL_TRC(Error);
      return COLrespondError(Address, Error);
   }
   COLrespondSuccess(Address, Result);
}

// /git/owners/update
void GITCupdateOwners(const COLwebRequest& Request) {
   COL_FUNCTION(GITCupdateOwners);
	bool Force = COLvarGetBool(Request.Data, "force", false);
	GITCupdateOwnersImpl(Request.User, Force, Request.Address);
}