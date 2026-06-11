#ifndef __WEB_SESSION_HANDLER_H__
#define __WEB_SESSION_HANDLER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2020 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBsessionHandler
//
// Description:
//
// Session handling APIs and functions
//
// Author: John Q
// Date:   Thu 21 May 2020 10:25:45 EDT
//---------------------------------------------------------------------------

#include <COL/COLclosure.h>
#include <WEB/WEBsession.h>

class WEBrequestData;
class WEBrequest;

bool WEBauthenticate(const COLstring& UserName, const COLstring& Password);
void WEBauthenticateSet(COLclosure3<COLstring, COLstring, bool*>* pCallback);

// TODO gross interface - squashy COLvar interface
bool WEBpermissions(const COLstring& Username, COLvar* pInfo);
void WEBpermissionsSet(COLclosure3<COLstring, COLvar*, bool*>* pCallback);

bool WEBwho(const COLmap<COLstring, WEBsession>& SessionMap, const WEBrequestData& Data, COLstring* pUser, bool* pAdminUser, COLvar* pPermissions);

bool WEBsessionNologin(const COLstring& Location);
bool WEBsessionAuthenticate(const WEBrequestData& Data, const COLmap<COLstring, WEBsession>& SessionMap, COLstring* pUser, bool* pAdminUser);

#endif // end of defensive include
