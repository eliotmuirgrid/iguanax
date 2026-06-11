// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITCsetToken
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Thursday 04 April 2024 - 01:00PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <GITC/GITCsetToken.h>
#include <GSV/GSVconfig.h>
#include <GSV/GSVmap.h>
COL_LOG_MODULE;

static bool GITCisEmailRequired(const GSVconfig& Config) {
   COL_FUNCTION(GITCisEmailRequired);
   return Config.m_ServerUrl == GSVdefaultHost(GIT_bitbucket_cloud) && (Config.m_Email.is_null() || Config.m_Email.isWhitespace());
}

// Gitlab does not require a username for setting a token
static bool GITCisUserRequired(const GSVconfig& Config) {
   COL_FUNCTION(GITCisUserRequired);
   return (Config.m_ServerUrl == GSVdefaultHost(GIT_bitbucket_cloud) ||
           Config.m_ServerUrl == GSVdefaultHost(GIT_azuredev_cloud)) &&
          (Config.m_Username.is_null() || Config.m_Username.isWhitespace());
}

// /git/token/set
void GITCsetToken(const COLwebRequest& Request) {
   COL_FUNCTION(GITCsetToken);
   COL_VAR(Request.Data);
   const COLstring Token = COLvarRequiredString(Request.Data, "token");
   const COLstring Email = COLvarGetString(Request.Data, "email", "");
   GSVconfig       Config;
   GSVconfigLoadUser(Request.User, &Config);
   COL_VAR(Config);
   if(GITCisUserRequired(Config)) { return COLrespondError(Request.Address, "Please setup the username first."); }
   Config.m_Token = Token;
   Config.m_Email = Email;
   if(Config.m_Email.is_null() || Config.m_Email.isWhitespace()) { GSVsetEmailFromConfig(Request.User, &Config); }
   if(GITCisEmailRequired(Config)) { return COLrespondError(Request.Address, "Please setup your email first."); }
   COLstring ErrorMsg;
   GSVconfigSaveUser(Request.User, Config, &ErrorMsg) ? COLrespondSuccess(Request.Address)
                                                      : COLrespondError(Request.Address, ErrorMsg);
}