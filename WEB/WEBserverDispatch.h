#ifndef __WEB_SERVER_DISPATCH_H__
#define __WEB_SERVER_DISPATCH_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBserverDispatch
//
// Description:
//
// Routine which handles dispatching of web API requests.
//
// Author: Eliot Muir
// Date:   Monday 22 May 2023 - 03:22PM
// ---------------------------------------------------------------------------

#include <COL/COLhashmap.h>
#include <COL/COLmap.h>
#include <COL/COLauto.h>
#include <COL/COLweb.h>

class WEBsession;
class COLstring;
class SCKloop;
class WEBrequestData;
class WEBserver;

void WEBserverDispatch(const WEBrequestData& RequestData,
                       COLmap<COLstring, WEBsession>* pSessionMap, SCKloop* pLoop,
                       bool RequireAuthentication,
                       COLhashmap<COLstring, COLwebHandlerFunc>& WorkerThreadHandlers,
                       COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& MainThreadHandlers,
                       WEBserver* pServer);

#endif // end of defensive include
