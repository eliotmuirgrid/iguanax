//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc. All Rights Reserved
//
//  Module: GRIDpeerComponents.cpp
//
//  Description: Polls peer component metrics using GRIDrequestAll
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLweb.h>
#include <GRID/GRIDcomponents.h>
#include <GRID/GRIDnode.h>
#include <GRID/GRIDrequest.h>
#include <GRID/GRIDserialize.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

GRIDcomponents::GRIDcomponents(GRIDnode* pNode, SCKloop* pLoop)
    : m_pNode(pNode), m_pLoop(pLoop), m_TimerId(-1), m_IntervalMs(10000) {
   COL_METHOD(GRIDpeerComponents::GRIDpeerComponents);
}

GRIDcomponents::~GRIDcomponents() {
   COL_METHOD(GRIDpeerComponents::~GRIDpeerComponents);
   stopPolling();
}

void GRIDcomponents::startOnMain() {
   COL_METHOD(GRIDstatsCollector::startOnMain);
   pollMetrics();
   m_TimerId = m_pLoop->addTimer(COLnewClosure0(this, &GRIDcomponents::pollMetrics), m_IntervalMs, true);
}

void GRIDcomponents::startPolling(const int IntervalMs) {
   COL_METHOD(GRIDpeerComponents::startPolling);
   if(m_TimerId != -1) {
      COL_TRC("Already polling, stopping first");
      stopPolling();
   }
   m_IntervalMs = IntervalMs;
   m_pLoop->post(COLnewClosure0(this, &GRIDcomponents::startOnMain));
   COL_TRC("Starting component metrics polling with interval=" << m_IntervalMs << "ms");
}

void GRIDcomponents::stopPolling() {
   COL_METHOD(GRIDpeerComponents::stopPolling);
   if(m_TimerId != -1) {
      COL_TRC("Stopping polling, timerId=" << m_TimerId);
      m_pLoop->cancelTimer(m_TimerId);
      m_TimerId = -1;
   }
}

void GRIDcomponents::pollMetrics() {
   COL_METHOD(GRIDpeerComponents::pollMetrics);
   if(!m_pNode || !m_pLoop) {
      COL_TRC("Node or Loop not available");
      return;
   }
   if(!m_pNode->validLicense()) {
      COL_TRC("Insufficient license");
      return;
   }
   GRIDrequestAll("/iguanas/components", COLvar(), m_pNode, m_pLoop, COLnewClosure1(this, &GRIDcomponents::handleResponse));
}

static GRIDcompMetrics GRIDextractMetrics(const COLvar& Data) {
   COL_FUNCTION(GRIDextractMetrics);
   GRIDcompMetrics Metrics;
   // make sure these match fields in GRIDAcomponents
   Metrics.CompsWithErr  = Data["errors"].isNull() ? -1 : Data["errors"].asInt();
   Metrics.MaxQueueCount = Data["queue"].isNull() ? -1 : Data["queue"].asInt();
   Metrics.Throughput    = Data["throughput"].isNull() ? -1 : Data["throughput"].asInt();
   return Metrics;
}

void GRIDcomponents::handleResponse(const COLvar& Results) const {
   COL_METHOD(GRIDpeerComponents::handleResponse);
   if(!m_pNode) return;
   COL_VAR(Results.json(1));
   for(const auto& It : m_pNode->getPeers()) {
      const COLstring& IguanaId   = It.first;
      const COLvar&    PeerResult = Results[IguanaId];
      if(PeerResult.isNull()) {
         COL_TRC("No valid response from " << IguanaId << ", marking metrics failed");
         m_pNode->markPeerMetricsFailed(IguanaId);
      } else {
         COL_TRC("Updating metrics for " << IguanaId);
         GRIDcompMetrics Metrics = GRIDextractMetrics(PeerResult);
         m_pNode->updatePeerMetrics(IguanaId, Metrics);
      }
   }
}
