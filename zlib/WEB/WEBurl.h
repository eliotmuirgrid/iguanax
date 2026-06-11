#ifndef __WEB_URL_H__
#define __WEB_URL_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBurl
//
// Description:
//
// Expression of URL which breaks it into parts to make it easier to consume.
//
// Author: Eliot Muir 
// Date:   Tuesday 28 March 2023 - 12:53PM
// ---------------------------------------------------------------------------

#include <COL/COLstring.h>
class WEBresponse;

class WEBurl{
public:
   bool IsHTTPS = false;
   int Port =80;
   COLstring Host;
   COLstring Path;
};

bool WEBurlParse(const COLstring& Url, WEBurl* pResult);
bool WEBtryUrlParse(WEBurl* pUrl, const COLstring& RequestUrl, WEBresponse* pOut);
#endif // end of defensive include
