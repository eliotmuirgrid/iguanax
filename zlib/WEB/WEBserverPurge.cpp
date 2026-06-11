// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBserverPurge
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Monday 22 May 2023 - 01:47PM
// ---------------------------------------------------------------------------

#include <WEB/WEBserverPurge.h>
#include <WEB/WEBserver.h>

#include <SCK/SCKloop.h>

#include <SCK/SCKconnection.h>
#include <SCK/SCKconnectionWrite.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

#define WEB_TIMEOUT_THRESHOLD 300 // connections idling for more than TIMEOUT_THRESHOLD second will be closed
#define WEB_TIMER_INTERVAL 20000 // idling connection check interval

static void WEBcloseConnectionIfIdle(SCKloop* pLoop, COLuint64 ConnectionId, WEBconnection* pConnection, time_t CurrentTime) {
   COL_FUNCTION(WEBcloseConnectionIfIdle);
   COL_VAR2(ConnectionId, pConnection);

   if(pConnection->m_IsWs) {
      COL_TRC("Connection " << ConnectionId << " is a websocket -- do not purge");
      return;
   }

   time_t LastActive = pConnection->m_LastActive;
   COL_VAR2(CurrentTime, LastActive);
   if (LastActive <= CurrentTime - WEB_TIMEOUT_THRESHOLD) {
      COL_TRC("Connection " << ConnectionId << " has been idling for more than " << WEB_TIMEOUT_THRESHOLD << "s. Closing the connection.");
      SCKconnectionHardClose(pLoop, ConnectionId);
  }
}

void WEBpurgeIdleConnections(SCKloop* pLoop, COLhashmap<COLuint64, WEBconnection*>* pConnectionMap) {
   COL_FUNCTION(WEBpurgeIdleConnections);
   COL_VAR(pConnectionMap);

   size_t NumberOfConnections = pConnectionMap->size();
   COL_VAR(NumberOfConnections);
   if (NumberOfConnections < 1) {
      COL_TRC("No established connections found.");
      return;
   }
   time_t CurrentTime = time(NULL);
   COLhashmapPlace From = pConnectionMap->first();
   while (From != NULL) {
      int ConnectionId = pConnectionMap->key(From);
      WEBconnection* pConnection = (*pConnectionMap)[From];
      From = pConnectionMap->next(From);
      WEBcloseConnectionIfIdle(pLoop, ConnectionId, pConnection, CurrentTime);
   }
}

int WEBpurgeTimerRun(SCKloop* m_pLoop, COLhashmap<COLuint64, WEBconnection*>* pConnections){
   COL_FUNCTION(WEBpurgeTimerRun);
   return m_pLoop->addTimer(COLnewClosure0(&WEBpurgeIdleConnections, m_pLoop, pConnections), WEB_TIMER_INTERVAL, true);
}
