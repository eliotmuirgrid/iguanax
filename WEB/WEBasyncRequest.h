#ifndef __WEB_ASYNC_REQUEST_H__
#define __WEB_ASYNC_REQUEST_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2020 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: WEBasyncRequest
//
// Author: Vismay Shah
//
// Description:
// Simple wrapper for WEBclient allowing for submission of asynchronous GET and POST requests that can handle
// both chunked and non-chunked data. Internally handles HTTP redirects and re-submits the 
// request to the appropriate URL. Will callback if a connection error is detected or once all 
// data is received. These are non-blocking requests.
//---------------------------------------------------------------------------
#include <COL/COLauto.h>
#include <COL/COLclosure.h>
class SCKloop;
class WEBresponse;
class WEBrequest;

void WEBrequestAsyncGet (const WEBrequest& Request, SCKloop* pLoop, COLclosure1<const WEBresponse&>* pCallback);
void WEBrequestAsyncPost(const WEBrequest& Request, SCKloop* pLoop, COLclosure1<const WEBresponse&>* pCallback);

#endif