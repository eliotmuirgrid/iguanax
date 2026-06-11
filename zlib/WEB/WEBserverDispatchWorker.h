#ifndef __WEB_SERVER_DISPATCH_WORKER_H__
#define __WEB_SERVER_DISPATCH_WORKER_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBserverDispatchWorker
//
// Description:
//
// Routine to dispatch web request to worker thread.
//
// Author: Eliot Muir
// Date:   Monday 22 May 2023 - 10:27PM
// ---------------------------------------------------------------------------

#include <COL/COLweb.h>
#include <COL/COLhashmap.h>


class SCKloop;
class WEBrequestData;
class WEBsession;

bool WEBserverDispatchWorker(const WEBrequestData& RequestData, COLhashmap<COLstring, COLwebHandlerFunc>& AsyncHandlers, COLstring User, bool AdminUser, SCKloop* pLoop);

// This is for the main thread.
bool WEBserverDispatchMain(const WEBrequestData& RequestData, COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& MainThreadHandlers, COLstring User, bool AdminUser);

#endif // end of defensive include
