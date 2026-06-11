// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDsession
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Monday 19 September 2022 - 07:06PM
// ---------------------------------------------------------------------------

#include <DBD/DBDsession.h>

#include <WEB/WEBsessionHandler.h>
#include <WEB/WEBserver.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void DBDsystemAuthenticateUser(COLstring Username, COLstring Password, bool* Valid, COLmap<COLstring, USRuser>*  pUserMap) {
   COL_FUNCTION(DBDsystemAuthenticateUser);
   *Valid = USRmapAuthenticate(pUserMap, Username, Password);
}

void DBDcreateSession(COLmap<COLstring, USRuser>*  pUserMap, WEBserver* pWebServer){
   COL_FUNCTION(DBDcreateSession);
   WEBauthenticateSet(COLnewClosure3(&DBDsystemAuthenticateUser, pUserMap));
}
