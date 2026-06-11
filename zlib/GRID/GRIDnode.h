#ifndef _GRIDNODE_H
#define _GRIDNODE_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDnode.h
//
//  Description: High-level node abstraction for grid connections.
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLclosure.h>
#include <COL/COLhashmap.h>
#include <COL/COLstring.h>
#include <GRID/GRIDcomponents.h>
#include <GRID/GRIDserialize.h>
#include <GRID/GRIDstats.h>
#include <GRID/GRIDwebSocket.h>

class SCKSsslContext;
class SCKloop;

class GRIDnode {
  public:
   struct GRIDapiConnection {
      COLclosure1<const COLvar&>* pCallback;
      int                         TimerId;
      explicit GRIDapiConnection(COLclosure1<const COLvar&>* pCb = nullptr, const int Id = -1) : pCallback(pCb), TimerId(Id) {}
   };

   GRIDnode(const GRIDconfig& Config, SCKloop& Loop);
   ~GRIDnode();

   void stop();
   bool IsStopping() const { return m_Stopping; }

   bool addPeer(const COLstring& IguanaId, const GRIDpeerInfo& PeerInfo, COLstring& Err);
   void removePeer(const COLstring& IguanaId);
   bool connect(const COLstring& IguanaId);
   void disconnect(COLstring IguanaId);

   void closeAllConnections();
   void acceptIncomingConnection(const COLstring& IguanaId, GRIDwebSocket* pConnection);

   void setReadAccess(const COLstring& IguanaId, GRIDreadAccessLevel Level);
   GRIDreadAccessLevel getReadAccess(const COLstring& IguanaId);
   
   void setLicenseCallback(COLauto<COLclosure1<COLvar*>> pCallback) { m_LicenseCallback = pCallback; }
   bool validLicense();

   void updatePeerMetrics(const COLstring& IguanaId, const GRIDcompMetrics& Metrics);
   void markPeerMetricsFailed(const COLstring& IguanaId);
   
   bool getCurrentStats(COLvar* pOut) const { return m_Collector.getCurrentStats(pOut); }

   void startComponentPolling() { m_PeerComponents.startPolling(); }
   void stopComponentPolling()  { m_PeerComponents.stopPolling(); }

   COLhashmap<COLuint64, GRIDapiConnection>&  getApiConnections() { return m_ApiConnections; }
   GRIDnodeInfo&                              getNodeInfo()       { return m_Config.m_NodeInfo; }
   const COLhashmap<COLstring, GRIDpeerInfo>& getPeers()          { return m_Config.m_PeerMap; }

  private:
   GRIDstatsCollector  m_Collector;
   GRIDcomponents      m_PeerComponents;
   GRIDconfig          m_Config;
   SCKloop&            m_Loop;
   bool                m_Stopping;

   COLhashmap<COLuint64, GRIDapiConnection> m_ApiConnections; // for remote/lua requests
   
   COLauto<COLclosure1<COLvar*>> m_LicenseCallback;

   bool hasHostPortConflict(const COLstring& IguanaId, const COLstring& Host, int Port);

   void onHeartbeat(const GRIDwebSocketHeartbeatFrame* pFrame, COLstring IguanaId);
   void onAuth(COLstring IguanaId);
   void onClose(COLstring IguanaId);
   void configure(const COLstring& IguanaId, GRIDwebSocket* pConnection);

   GRIDnode(const GRIDnode&)            = delete;
   GRIDnode& operator=(const GRIDnode&) = delete;
};

#endif
