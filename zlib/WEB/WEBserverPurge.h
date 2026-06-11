#ifndef __WEB_SERVER_PURGE_H__
#define __WEB_SERVER_PURGE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBserverPurge
//
// Description:
//
// Code to purge old unused web connections.
//
// Author: Eliot Muir 
// Date:   Monday 22 May 2023 - 01:47PM
// ---------------------------------------------------------------------------

#include <COL/COLhashmap.h>
class WEBconnection;
class SCKloop;

void WEBpurgeIdleConnections(COLhashmap<COLuint64, WEBconnection*>* pConnectionMap);
int WEBpurgeTimerRun(SCKloop* m_pLoop, COLhashmap<COLuint64, WEBconnection*>* pConnections);

#endif // end of defensive include
