#ifndef __WEB_LOGIN_H__
#define __WEB_LOGIN_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBlogin
//
// Description:
//
// Seperated login concern for easier tracing
//
// Author: Eliot Muir
// Date:   Thu  1 Sep 2022 12:00:18 EDT
//---------------------------------------------------------------------------

#include <COL/COLclosure.h>
#include <COL/COLmap.h>
class COLstring;
class COLwebRequest;
class COLvar;
class USRuser;
class WEBsession;

// calling this function with SessionId = "" will instruct the client (browser) to delete the session cookie
void WEBresponseWithIguanaSessionCookie(const COLvar& Data, const COLstring& SessionId, bool Secure,COLstring* pResult);

void WEBlogin(const COLwebRequest& Request, COLmap<COLstring, WEBsession>* pSessionMap, COLmap<COLstring, USRuser>* pUserMap, COLclosure3<COLstring, COLstring, COLclosure1<COLvar>*>* pExternalAuthenticator);

#endif // end of defensive include
