#ifndef _GRIDCONNECTIONMONITOR_H
#define _GRIDCONNECTIONMONITOR_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDconnectionMonitor.h
//
//  Description: Manages heartbeat and ping/pong monitoring for connections
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLauto.h>
#include <COL/COLclosure.h>

class SCKloop;

class GRIDconnectionMonitor {
  public:
   GRIDconnectionMonitor(SCKloop& Loop);
   ~GRIDconnectionMonitor();

   void startHeartbeat(COLauto<COLclosure0> pCallback);
   void stopHeartbeat();
   
   void recordPingSent();
   void recordPingReceived();
   COLuint64 getLastRtt() const { return m_LastRtt; }

  private:
   SCKloop&  m_Loop;
   int       m_HeartbeatTimerId = -1;
   COLuint64 m_LastPingSentTime = 0;
   COLuint64 m_LastRtt = 0;

   GRIDconnectionMonitor(const GRIDconnectionMonitor&) = delete;
   GRIDconnectionMonitor& operator=(const GRIDconnectionMonitor&) = delete;
};

#endif
