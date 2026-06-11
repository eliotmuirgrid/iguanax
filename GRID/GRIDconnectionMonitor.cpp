//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDconnectionMonitor.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLerror.h>
#include <COL/COLlog.h>
#include <GRID/GRIDconnectionMonitor.h>
#include <SCK/SCKloop.h>
#include <chrono>
COL_LOG_MODULE;

static COLuint64 GRIDgetTimeMs() {
   return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

GRIDconnectionMonitor::GRIDconnectionMonitor(SCKloop& Loop)
    : m_Loop(Loop), m_HeartbeatTimerId(-1), m_LastPingSentTime(0), m_LastRtt(0) {
   COL_METHOD(GRIDconnectionMonitor::GRIDconnectionMonitor);
}

GRIDconnectionMonitor::~GRIDconnectionMonitor() {
   COL_METHOD(GRIDconnectionMonitor::~GRIDconnectionMonitor);
   stopHeartbeat();
}

void GRIDconnectionMonitor::startHeartbeat(COLauto<COLclosure0> pCallback) {
   COL_METHOD(GRIDconnectionMonitor::startHeartbeat);
   if(m_HeartbeatTimerId == -1) {
      COL_TRC("Starting heartbeat timer with 5s interval");
      pCallback->run();
      m_HeartbeatTimerId = m_Loop.addTimer(pCallback, 5000, true);
   } else {
      COL_TRC("Heartbeat timer already running, id=" << m_HeartbeatTimerId);
   }
}

void GRIDconnectionMonitor::stopHeartbeat() {
   COL_METHOD(GRIDconnectionMonitor::stopHeartbeat);
   if(m_HeartbeatTimerId != -1) {
      COL_TRC("Stopping heartbeat timer, id=" << m_HeartbeatTimerId);
      // Validate timer ID is in reasonable range before attempting cancel
      if(m_HeartbeatTimerId > 0 && m_HeartbeatTimerId < 1000000) {
         try {
            m_Loop.cancelTimer(m_HeartbeatTimerId);
         } catch(const COLerror& E) {
            COL_TRC("Exception canceling timer: " << E.description());
         }
      } else {
         COL_TRC("Timer " << m_HeartbeatTimerId << " not found in loop, skipping cancel");
      }
      m_HeartbeatTimerId = -1;
   }
}

void GRIDconnectionMonitor::recordPingSent() {
   COL_METHOD(GRIDconnectionMonitor::recordPingSent);
   m_LastPingSentTime = GRIDgetTimeMs();
   COL_TRC("Recorded ping sent at " << m_LastPingSentTime);
}

void GRIDconnectionMonitor::recordPingReceived() {
   COL_METHOD(GRIDconnectionMonitor::recordPingReceived);
   if(m_LastPingSentTime > 0) {
      const COLuint64 NowMs = GRIDgetTimeMs();
      m_LastRtt = NowMs - m_LastPingSentTime;
      COL_TRC("Ping RTT: " << m_LastRtt << "ms (sent=" << m_LastPingSentTime << ", now=" << NowMs << ")");
      m_LastPingSentTime = 0;
   } else {
      COL_TRC("Ignoring pong - no pending ping");
   }
}
