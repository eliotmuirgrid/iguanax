// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITCsshValid
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Friday 05 April 2024 - 02:28PM
// ---------------------------------------------------------------------------
#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLweb.h>
#include <FIL/FILutils.h>
#include <GITU/GITUcache.h>	// for GIT cache refresh and Ssh in progress checks
#include <GITU/GITUconfigGlobal.h>
#include <GITU/GITUsshkeygenDetect.h>	 // for GITsshDetect
#include <GITC/GITCdefaultKeyPair.h>
#include <GITC/GITCsshValid.h>
#include <GITE/GITEazureDevops.h>
#include <GITE/GITEbitbucket.h>
#include <GITE/GITEgithub.h>
#include <GITE/GITEgitlab.h>
#include <GSV/GSVconfig.h>
#include <GSV/GSVmap.h>
#include <PRO/PROexecute.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

static void GITCsshCheckAccessComplete(){
   COL_FUNCTION(GITCsshCheckAccessComplete);
   COL_TRC("SSH access check complete.");
}

bool GITCsshCheckAccess(const COLstring& IguanaUser, const GSVconfig& Config, const GSVitem& Item,
								COLstring* pError) {
   COL_FUNCTION(GITCsshCheckAccess);
   switch (Item.m_Type){
      case GIT_bitbucket_cloud: 
      case GIT_bitbucket_server:
         return GITEbitbucketCheckSshAccess(IguanaUser, Config.m_Username, Config, pError);
      case GIT_gitlab_cloud:    
      case GIT_gitlab_server:
         return GITEgitlabCheckSshAccess(IguanaUser, Config, pError);
      case GIT_azuredev_cloud:  
      case GIT_azuredev_server:
         return GITEazureCheckSshAccess(IguanaUser, Config.m_Username, Config, pError);
      case GIT_github_cloud:    
      case GIT_github_server:
         return GITEgithubCheckSshAccess(IguanaUser, Config, pError);
      default:
         // not implemented
         *pError = "not yet implemented for host: " + Config.m_ServerUrl;
         return false;
   }
}

static void GITCsshValidImpl(COLstring Username, COLaddress Address, COLclosure0* pCallback, SCKloop* pLoop) {
   COL_FUNCTION(GITCsshValidImpl);
   COLvar Response;
   Response["valid"] = false;
   GSVconfig Config;
   if (!GSVconfigLoadUser(Username, &Config)){
      COLrespondError(Address, "Cannot check for ssh access, failed to load user's git configuration");
      return pLoop->post(pCallback);
   }
	if(!GSVmapExists(Config.m_ServerUrl)) {
      COLrespondError(Address, Config.m_ServerUrl + " not found in server map");
      return pLoop->post(pCallback);
	}
	const GSVitem Item = GSVmapItem(Config.m_ServerUrl);
   if ((Item.m_Type == GIT_bitbucket_cloud && Config.m_Username.is_null()) ){
      COLrespondError(Address, "Cannot check for ssh access, no username is configured");
      return pLoop->post(pCallback);
   }
   COLstring PrivateKey = GITUconfigPrivateKey(Username);
   COLstring PrivateKeyFile = GITUstripPathsQuotes(PrivateKey);
   // TODO - IX-3403
   Response["set"] = FILfileExists(PrivateKeyFile);
   COL_TRC("Found a private key file, checking for ssh access");
   COLstring ErrorMessage;
   Response["valid"]   = GITCsshCheckAccess(Username, Config, Item, &ErrorMessage);
   Response["message"] = ErrorMessage;
   COL_VAR(Response);
   COLrespondSuccess(Address, Response);
   pLoop->post(pCallback);
}

// git/ssh/valid
void GITCsshValid(const COLwebRequest& Request, bool* pGitCacheRefreshing, SCKloop* pLoop) {
   COL_FUNCTION(GITCsshValid);
   COLstring Username = Request.User;
   COLstring Error;
   COLclosure0* pCallback = COLnewClosure0(&GITCsshCheckAccessComplete);
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&GITCsshValidImpl, Username, Request.Address, pCallback, pLoop));
}
