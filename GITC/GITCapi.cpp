// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GITCapi.cpp
//
//  Description
//
//  Author: Matthew Sobkowski
//  Date:   10/19/22 12:49 PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <GITC/GITCallUrls.h>
#include <GITC/GITCapi.h>
#include <GITC/GITCcheckRepoPath.h>
#include <GITC/GITCcredentials.h>
#include <GITC/GITCdefaultKeyPair.h>
#include <GITC/GITCgenerateDefaultKeyPair.h>
#include <GITC/GITCkeyTypeOverride.h>
#include <GITC/GITCkeyPathReset.h>
#include <GITC/GITCkeysSet.h>
#include <GITC/GITCorganizations.h>
#include <GITC/GITCorganizationsSet.h>
#include <GITC/GITCowners.h>
#include <GITC/GITCserver.h>
#include <GITC/GITCserversLoad.h>
#include <GITC/GITCserversSave.h>
#include <GITC/GITCsetCredentials.h>
#include <GITC/GITCsetServer.h>
#include <GITC/GITCsetSsh.h>
#include <GITC/GITCsetToken.h>
#include <GITC/GITCsetUsername.h>
#include <GITC/GITCsshValid.h>
#include <GITC/GITCtokenValid.h>
#include <GITC/GITCupdateOwners.h>
#include <GITC/GITCusername.h>
#include <WEB/WEBserver.h>
COL_LOG_MODULE;

void GITCworker(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap){
   COL_FUNCTION(GITCworker);
   ApiMap.add("/git/reset_key_path",            &GITCkeyPathReset);
   ApiMap.add("/git/private_key_path",          &GITCdefaultKeyPair);
   ApiMap.add("/git/generate_default_key_pair", &GITCgenerateDefaultKeyPair);
   ApiMap.add("/git/credentials",               &GITCcredentials);
   ApiMap.add("/git/credentials/set",           &GITCsetCredentials);
	ApiMap.add("/git/check_repo_path",           &GITCcheckRepoPath);
	ApiMap.add("/git/all_urls",                  &GITCallUrls);
	ApiMap.add("/git/server",                    &GITCserver);
	ApiMap.add("/git/token/valid",               &GITCtokenValid);
	ApiMap.add("/git/owners/update",             &GITCupdateOwners);
	ApiMap.add("/git/ssh/set",                   &GITCsetSsh);
	ApiMap.add("/git/owners",                    &GITCowners);
   ApiMap.add("/git/username",                  &GITCusername);
   ApiMap.add("/git/username/set",              &GITCsetUsername);
   ApiMap.add("/git/token/set",                 &GITCsetToken);
   ApiMap.add("/git/server/set",                &GITCsetServer);
	ApiMap.add("/git/servers/load",              &GITCserversLoad);
	ApiMap.add("/git/servers/save",              &GITCserversSave);
	ApiMap.add("/git/key_type/override",         &GITCkeyTypeOverride);
   //ApiMap.add("/git/organizations",             &GITCorganizations);
   ApiMap.add("/git/organizations/set",         &GITCorganizationsSet);
}

void GITCmain(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap,
     bool* pGitCacheRefreshing, COLmap<COLstring, USRuser>* pUserMap, SCKloop* pLoop) {
   COL_FUNCTION(GITCmain);
   ApiMap.add("/git/ssh/valid", COLnewClosure1(&GITCsshValid, pGitCacheRefreshing, pLoop));
   ApiMap.add("/git/keys/set",  COLnewClosure1(&GITCkeysSet, pUserMap, pLoop));
}