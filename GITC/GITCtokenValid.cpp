// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITCtokenValid
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Thursday 04 April 2024 - 02:45PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLweb.h>
#include <GITE/GITEazureDevops.h>
#include <GITE/GITEbitbucket.h>
#include <GITE/GITEgithub.h>
#include <GITE/GITEgitlab.h>
#include <GSV/GSVconfig.h>
#include <GSV/GSVmap.h>
COL_LOG_MODULE;

static bool GITCverifyToken(const GSVconfig& Config, const GSVitem& Item, int& Code, COLstring* pError) {
   COL_FUNCTION(GITCverifyToken);
   switch(Item.m_Type) {
      case GIT_azuredev_cloud:
      case GIT_azuredev_server: return GITEazureCheckToken(Config, Item.m_Secure, Code, pError);
      case GIT_bitbucket_cloud:
      case GIT_bitbucket_server: return GITEbitbucketCheckToken(Config.m_Token, Config, Code, pError);
      case GIT_gitlab_cloud:
      case GIT_gitlab_server: return GITEgitlabCheckToken(Config.m_Token, Config, Code, pError);
      case GIT_github_cloud:
      case GIT_github_server: return GITEgithubCheckToken(Config.m_Token, Config, Code, pError);
      default: return false;
   }
}

static bool GITCtokenIsSet(const GSVconfig& Config){
   COL_FUNCTION(GITCtokenIsSet);
   return (!Config.m_Token.is_null() && !Config.m_Token.isWhitespace());
}

static bool GITCtokenIsValid(const GSVconfig& Config, const GSVitem& Item, int& TokenCode, COLstring* pTokenError){
   COL_FUNCTION(GITCtokenIsValid);
   return GITCverifyToken(Config, Item, TokenCode, pTokenError);
}

static void GITCtokenValidImpl(const COLstring& Username, const COLaddress& Address) {
   COL_FUNCTION(GITCtokenValidImpl);
   GSVconfig Config;
   GSVconfigLoadUser(Username, &Config);
   if(!GSVmapExists(Config.m_ServerUrl)) {
      return COLrespondError(Address, Config.m_ServerUrl + " not found in list of servers");
   }
   const GSVitem Item = GSVmapItem(Config.m_ServerUrl);
   COLvar        Response;
   int           TokenCheckCode;
   COLstring     TokenError;
   Response["deprecated"] = GITEbitbucketIsDeprecatedToken(Config.m_Token);
   Response["email"]      = Config.m_Email;
   Response["valid"]      = false;
   Response["access"]     = true;
   Response["set"]        = GITCtokenIsSet(Config);
   if(Response["set"]) { Response["valid"] = GITCtokenIsValid(Config, Item, TokenCheckCode, &TokenError); }
   if(TokenCheckCode >= 500) { Response["access"] = false; }  // connection failure, ex: no internet access.
   Response["message"] = TokenError;
   COLrespondSuccess(Address, Response);
}

// /git/token/valid
void GITCtokenValid(const COLwebRequest& Request) {
   COL_FUNCTION(GITCtokenValid);
   GITCtokenValidImpl(Request.User, Request.Address);
}