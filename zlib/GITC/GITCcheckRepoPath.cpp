// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITCcheckRepoPath
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Tuesday 09 April 2024 - 04:21PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <GITC/GITCcheckRepoPath.h>
#include <GITE/GITEazureDevops.h>
#include <GITE/GITEbitbucket.h>
#include <GITE/GITEgithub.h>
#include <GITE/GITEgitlab.h>
#include <GSV/GSVconfig.h>
#include <GSV/GSVmap.h>
#include <GSV/GSVlink.h>
COL_LOG_MODULE;

static void GITCcheckRepoImpl(const COLstring& Username, const COLstring& Slug, const COLstring& Owner,
										const COLaddress& Address) {
	COL_FUNCTION(GITCcheckRepoImpl);
	COL_VAR3(Username, Slug, Owner);
	COLvar Result;
	COLstring Error;
	GSVconfig Config;
	GSVconfigLoadUser(Username, &Config);
	COL_VAR(Config.m_ServerUrl);
	bool Success = false;
	GSVitem Item;
	if(!GSVmapExists(Config.m_ServerUrl)) {
		Error = Config.m_ServerUrl + " not found in server list";
	} else {
		Item = GSVmapItem(Config.m_ServerUrl);
		switch(Item.m_Type) {
			case GIT_azuredev_cloud:  
			case GIT_azuredev_server:
				Success = GITEazureCheckRepo	 (Owner, Slug, Config, Item.m_Secure, &Result, &Error); 
				break;
			case GIT_bitbucket_cloud: 
			case GIT_bitbucket_server:
				Success = GITEbitbucketCheckRepo(Owner, Slug, Config, &Result, &Error); 
				break;
			case GIT_gitlab_cloud: 	  
			case GIT_gitlab_server:
				Success = GITEgitlabCheckRepo	 (Owner, Slug, Config, &Result, &Error); 
				break;
			case GIT_github_cloud: 	  
			case GIT_github_server:
				Success = GITEgithubCheckRepo	 (Owner, Slug, Config, &Result, &Error); 
				break;
			default: break;
		}
	}

	if(!Success) {
		COL_TRC(Error);
		return COLrespondError(Address, Error);
	}
	const COLstring Prefix	 = Item.m_Secure ? "https://" : "http://";
	COLstring InputUrl = Prefix + Config.m_ServerUrl + "/" + Owner + "/";
	Item.m_Type == GIT_azuredev_cloud && Config.m_Local ? InputUrl += "_git/" + Slug : InputUrl += Slug;
	COL_VAR(InputUrl);
	GSVlink Link;
	GSVconstructLink(InputUrl, &Link);
	Result["urls"] = GSVlinkToVar(Link);
	COL_VAR(Result);
	COLrespondSuccess(Address, Result);
}

// /git/check_repo_path
void GITCcheckRepoPath(const COLwebRequest& Request) {
   COL_FUNCTION(GITCcheckRepoPath);
	COLstring Slug	 = COLvarRequiredString(Request.Data, "slug");
	COLstring Owner = COLvarRequiredString(Request.Data, "owner");
	COL_TRC("Checking owner string");
   if (Owner.is_null() || Owner.isWhitespace()) { return COLrespondError(Request.Address, "Must provide owner"); }
   COL_TRC("Checking slug string");
   if (Slug.is_null() || Slug.isWhitespace()) {
      COLvar Result;
      Result["slug_exists"] = false;
      return COLrespondSuccess(Request.Address, Result);
   }
   COL_TRC("Owner and slug are defined, continue with web requests");
	GITCcheckRepoImpl(Request.User, Slug, Owner, Request.Address);
}

