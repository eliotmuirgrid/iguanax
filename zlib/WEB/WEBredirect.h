#ifndef __WEB_REDIRECT_H__
#define __WEB_REDIRECT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBredirect
//
// Description:
//
// Redirect HTTP client to the HTTPS site when HTTPS is enabled
//
// Author: John Q
// Date:   Thu 31 Aug 2023 13:36:22 PDT
// ---------------------------------------------------------------------------

class COLstring;
class SCKconnection;

bool WEBisHttpRequest(const COLstring& Buffer);
void WEBserveRedirectHtml(SCKconnection* pConnection, const COLstring& Data);

// could fail in case Buffer doesn't contain the request URI and/or the Host header
bool WEBautoRedirect(const COLstring& Buffer, bool ToHTTPS, SCKconnection* pConnection);

#endif // end of defensive include
