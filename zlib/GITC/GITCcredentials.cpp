// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GITCcredentials
//
//  Description
//
// Author: Tyler Brown
// Date:   29 Dec 2022
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <GITU/GITUconfigGlobal.h>
#include <GITC/GITCcredentials.h>
#include <GITU/GITUconfigGlobal.h>
#include <INI/INIdata.h>
COL_LOG_MODULE;

void GITCgetUserEmail(const COLstring& Username, COLvar* pGitConfig) {
   COL_FUNCTION(GITCgetUserEmail);
   COLvar& Config = *pGitConfig;
   INIdata UserConfig;
   COLstring User, Email;
   bool Success = GITUgetUserEmail(Username, &User, &Email);
   if(!Success){
      COL_TRC("Initializing git user configuration file");
      UserConfig = GITUinitializeConfigUser(Username);
      GITUinitializeUsersPrivateKey(Username, UserConfig);
      Success = GITUgetUserEmail(Username, &User, &Email);
   }
   if(!Success){
      COL_TRC("Failed to get user email");
      return;
   }
   Config["name"]  = User;
   Config["email"] = Email;
}

void GITCcredentialsImpl(const COLstring& Username, const COLaddress& Address) {
   COL_FUNCTION(GITCcredentialsImpl);
   COLvar Data;
   GITCgetUserEmail(Username, &Data);
   COL_VAR(Data.json(1));
   COLrespondSuccess(Address, Data);
}

// /git/credentials 
void GITCcredentials(const COLwebRequest& Request) {
   COL_FUNCTION(GITCcredentials);
   const COLvar& Data = Request.Data;
   COL_VAR(Data);
   COLstring Username = Request.User;
   GITCcredentialsImpl(Username, Request.Address);
}
