#ifndef __WEB_COOKIE_H__
#define __WEB_COOKIE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBcookie
//
// Description:
//
// Extract specific cookie directly from header.  In practice we only
// use session cookies so we don't need much parsing logic.
//
// Author: Eliot Muir
// Date:   Saturday 20 May 2023 - 02:13PM
// ---------------------------------------------------------------------------

#include <COL/COLmap.h>
class COLstring;

// Enables the multiple iguana session cookies to coexist in a web browser.
void WEBsetCookieHost(const COLstring& HostId);

// returns the session cookie name depending on it's for HTTP or HTTPS
COLstring WEBdefaultCookieName(bool Https);

COLstring WEBcookieGet(const COLmap<COLstring, COLstring>& Headers, const COLstring& CookieName);

#endif // end of defensive include
