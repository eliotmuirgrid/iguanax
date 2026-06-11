// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITCkeyPathReset
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Monday 29 April 2024 - 10:29AM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <GITU/GITUconfigGlobal.h>
#include <GITC/GITCkeyPathReset.h>
#include <INI/INIdata.h>
COL_LOG_MODULE;

void GITCkeyPathResetImpl(const COLstring& Username, const COLaddress& Address){
   COL_FUNCTION(GITCkeyPathResetImpl);
   INIdata Config;
   GITUconfigLoadUser(Username, &Config);
   COLstring PrivateKey = DIRconfigUserSshFile(Username);
   GITUconfigSetSshPath(&Config, PrivateKey);
   GITUconfigSaveUser(Username, Config);
   COLrespondSuccess(Address);
}

// /git/reset_key_path
void GITCkeyPathReset(const COLwebRequest& Request) {
   COL_FUNCTION(GITCkeyPathReset);
   COLstring Username = Request.User;
   COLvar Result;
   GITCkeyPathResetImpl(Username, Request.Address);
}
