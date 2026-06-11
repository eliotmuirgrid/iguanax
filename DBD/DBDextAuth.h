#ifndef __DBD_EXT_AUTH_H__
#define __DBD_EXT_AUTH_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDextAuth
//
// Description:
//
// Functions for external authentications such as LDAP.
//
// Author: Eliot Muir
// Date:   Monday 19 September 2022 - 12:57PM
// ---------------------------------------------------------------------------

#include <COL/COLclosure.h>
#include <COL/COLmap.h>
#include <COL/COLstring.h>
#include <COL/COLvar.h>

class BLUAinstanceManager;
struct CFGmap;
class SCKloop;
class WEBsession;
class USRuser;

#define DBD_EXT_AUTH_COMPONENT_ENV_NAME "IFW_EAUTH_COMPONENT"
#define DBD_EXT_AUTH_POLL_TIME_ENV_NAME "IFW_EAUTH_POLL_INTERVAL"
#define DBD_EXT_AUTH_DEFAULT_POLL_TIME_IN_SECONDS  60

void DBDextAuthenticateUser(COLstring Username, COLstring Password, COLclosure1<COLvar>* pCallback, BLUAinstanceManager* pManager, CFGmap* pMap, SCKloop* pLoop);
void DBDextAuthSetupRefresh(BLUAinstanceManager* pManager, CFGmap* pComponentMap, COLmap<COLstring, USRuser>* pUserMap, COLmap<COLstring, WEBsession>* pSessionMap, SCKloop* pLoop, int* pTimerId);
void DBDextAuthTearDownRefresh(SCKloop* pLoop, int& TimerId);

#endif // end of defensive include
