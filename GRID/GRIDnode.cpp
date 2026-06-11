//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDnode.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLclosure.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <GRID/GRIDhandleJsonReqRes.h>
#include <GRID/GRIDnode.h>
#include <IWEB/IWEBconfig.h>
#include <SCK/SCKloop.h>
#include <SCKS/SCKSsslContext.h>
#include <TUN/TUNkey.h>
COL_LOG_MODULE;

static void GRIDcaptureConnectionError(GRIDpeerInfo* pInfo, const GRIDwebSocket* pConnection) {
   COL_FUNCTION(GRIDcaptureConnectionError);
   const int       ErrorCode   = pConnection->getLastErrorCode();
   const COLstring ErrorString = pConnection->getLastErrorString();
   if(ErrorCode == 0) { return; }
   pInfo->LastErrorCode = ErrorCode;
   pInfo->LastError     = ErrorString;
   pInfo->LastErrorTime = time(nullptr);
   COL_TRC("Captured connection error: code=" << ErrorCode << " message=" << pInfo->LastError);
}

static void GRIDclearConnectionError(GRIDpeerInfo* pInfo) {
   COL_FUNCTION(GRIDclearConnectionError);
   pInfo->LastErrorCode = 0;
   pInfo->LastError.clear();
   pInfo->LastErrorTime = 0;
}

GRIDnode::GRIDnode(const GRIDconfig& Config, SCKloop& Loop)
    : m_Collector(&Loop), m_PeerComponents(this, &Loop), m_Config(Config), m_Loop(Loop), m_Stopping(false) {
   COL_METHOD(GRIDnode::GRIDnode);
}

GRIDnode::~GRIDnode() {
   COL_METHOD(GRIDnode::~GRIDnode);
   const size_t LuaCallbackCount = m_ApiConnections.size();
   for(auto It = m_ApiConnections.begin(); It != m_ApiConnections.end(); ++It) { if(It->second.pCallback) delete It->second.pCallback; }
   m_ApiConnections.clear();
   COL_TRC("Cleaned up " << LuaCallbackCount << " Lua connection callbacks");
   stop();
}

void GRIDnode::stop() {
   COL_METHOD(GRIDnode::stop);
   m_Stopping = true;
   m_PeerComponents.stopPolling();
   closeAllConnections();
   m_Config.m_PeerMap.clear();
   delete m_Config.m_NodeInfo.m_pPrivateKey;
   delete m_Config.m_NodeInfo.m_pContext;
   COL_TRC("Node stopped");
}

bool GRIDnode::addPeer(const COLstring& IguanaId, const GRIDpeerInfo& PeerInfo, COLstring& Err) {
   COL_METHOD(GRIDnode::addPeer);
   if(m_Config.m_NodeInfo.m_IguanaId.is_null()) {
      COL_TRC("Cannot add Iguana without identity set");
      Err = "This instance is not configured for HA.";
      return false;
   }
   auto& Peers = m_Config.m_PeerMap;

   if(hasHostPortConflict(IguanaId, PeerInfo.Host, PeerInfo.Port)) {
      Err = "Another peer is already connected to this host and port.";
      return false;
   }

   auto PeerIt = Peers.find(IguanaId);
   const bool Exists = PeerIt != Peers.end();
   if(Exists) {
      if(PeerIt->second.pConnection && PeerIt->second.pConnection->connected()) {
         COL_TRC("Already connected to Iguana " << IguanaId);
         Err = "You are already connected to an instance at this address.";
         return false;
      }
      if(PeerIt->second.pConnection) {
         COL_TRC("Cleaning up previous connection attempt for Iguana " << IguanaId);
         PeerIt->second.pConnection->setOnClose(nullptr);
         delete PeerIt->second.pConnection;
         PeerIt->second.pConnection = nullptr;
      }
   }

   auto pConnection            = new GRIDwebSocket(m_Config.m_NodeInfo, PeerInfo.Fingerprint, m_Loop);
   Peers[IguanaId]             = PeerInfo;
   Peers[IguanaId].pConnection = pConnection;
   configure(IguanaId, pConnection);
   return true;
}

void GRIDnode::removePeer(const COLstring& IguanaId) {
   COL_METHOD(GRIDnode::removePeer);
   const auto ConnIt = m_Config.m_PeerMap.find(IguanaId);
   if(ConnIt != m_Config.m_PeerMap.end()) {
      m_Config.m_PeerMap.erase(ConnIt);
      COL_TRC("Removed connection " << IguanaId);
   }
}

bool GRIDnode::connect(const COLstring& IguanaId) {
   COL_METHOD(GRIDnode::connect);
   COL_VAR(IguanaId);
   auto ConnIt = m_Config.m_PeerMap.find(IguanaId);
   if(ConnIt == m_Config.m_PeerMap.end()) { COL_TRC("Cannot connect to unknown peer"); return false; }

   if(hasHostPortConflict(IguanaId, ConnIt->second.Host, ConnIt->second.Port)) { return false; }

   auto* pConn = ConnIt->second.pConnection;
   if(pConn && pConn->getSocket() && pConn->connected()) { COL_TRC("Already connected."); return false; }
   

   if(pConn) {
      COL_TRC("Cleaning up existing connection for Iguana " << IguanaId);
      pConn->setOnClose(nullptr);
      delete pConn;
   }
   ConnIt->second.pConnection = new GRIDwebSocket(m_Config.m_NodeInfo, ConnIt->second.Fingerprint, m_Loop);
   configure(IguanaId, ConnIt->second.pConnection);
   ConnIt->second.pConnection->connect(ConnIt->second.Host, ConnIt->second.Port);
   COL_TRC("Connecting to peer, UseHttps=" << m_Config.m_NodeInfo.m_pWebConfig->useHttps());
   return true;
}

void GRIDnode::disconnect(const COLstring IguanaId) {
   COL_METHOD(GRIDnode::disconnect);
   const auto it = m_Config.m_PeerMap.find(IguanaId);
   if(it == m_Config.m_PeerMap.end()) {
      COL_TRC(IguanaId << " wasn't connected");
      return;
   }
   onClose(IguanaId);
}

void GRIDnode::closeAllConnections() {
   COL_METHOD(GRIDnode::closeAllConnections);
   for(auto it = m_Config.m_PeerMap.begin(); it != m_Config.m_PeerMap.end(); ++it) {
      if(it->second.pConnection) {
         it->second.pConnection->setOnClose(nullptr);
         it->second.pConnection->close();
         delete it->second.pConnection;
         it->second.pConnection = nullptr;
      }
   }
   COL_TRC("All connections closed");
}

void GRIDnode::acceptIncomingConnection(const COLstring& IguanaId, GRIDwebSocket* pConnection) {
   COL_METHOD(GRIDnode::acceptIncomingConnection);
   if(m_Config.m_NodeInfo.m_IguanaId.is_null()) {
      COL_TRC("Cannot accept connection without identity set");
      delete pConnection;
      return;
   }

   auto PeerIt = m_Config.m_PeerMap.find(IguanaId);
   if(PeerIt == m_Config.m_PeerMap.end()) {
      COL_TRC("Accepting incoming connection from " << IguanaId);
      m_Config.m_PeerMap[IguanaId].pConnection = pConnection;
      return configure(IguanaId, pConnection);
   }

   auto pConn = PeerIt->second.pConnection;
   if(pConn) {
      if(pConn->connected()) {
         COL_TRC("Already have incoming live connection from " << IguanaId);
         pConnection->close();
         delete pConnection;
         return;
      }
      COL_TRC("Removing stale connection to be replaced with new");
      pConn->setOnClose(nullptr);
      delete pConn;
   }
   PeerIt->second.pConnection = pConnection;
   configure(IguanaId, pConnection);
}

bool GRIDnode::hasHostPortConflict(const COLstring& IguanaId, const COLstring& Host, int Port) {
   COL_METHOD(GRIDnode::hasHostPortConflict);
   for(const auto& It : m_Config.m_PeerMap) {
      if(It.first != IguanaId && It.second.Host == Host && It.second.Port == Port) {
         if(It.second.pConnection && (It.second.pConnection->connected() || It.second.pConnection->getSocket())) {
            COL_TRC("Host/port conflict: peer " << It.first << " already at " << Host << ":" << Port);
            return true;
         }
      }
   }
   return false;
}

void GRIDnode::onHeartbeat(const GRIDwebSocketHeartbeatFrame* pFrame, COLstring IguanaId) {
   COL_METHOD(GRIDnode::onHeartbeat);
   COL_TRC("Heartbeat received from: " << pFrame->m_InstanceName << " id=" << IguanaId);
   auto ConnIt = m_Config.m_PeerMap.find(IguanaId);
   if(ConnIt == m_Config.m_PeerMap.end()) { COL_TRC(IguanaId << " not found,"); return; }
   if(pFrame->m_InstanceName.is_null())   { return; }

   if(ConnIt->second.InstanceName != pFrame->m_InstanceName) {
      COL_TRC("Updating client connection IguanaId for " << IguanaId << " -> " << pFrame->m_InstanceName);
      ConnIt->second.InstanceName = pFrame->m_InstanceName;
   }
}

static void GRIDdeleteWebSocket(GRIDwebSocket* pConn) { delete pConn; }

void GRIDnode::onClose(const COLstring IguanaId) {
   COL_METHOD(GRIDnode::onClose);
   auto ConnIt = m_Config.m_PeerMap.find(IguanaId);
   if(ConnIt == m_Config.m_PeerMap.end()) {
      COL_TRC("Peer not found for cleanup: " << IguanaId);
      return;
   }
   if(ConnIt->second.pConnection) {
      GRIDcaptureConnectionError(&ConnIt->second, ConnIt->second.pConnection);
      ConnIt->second.pConnection->setOnClose(nullptr);
      GRIDwebSocket* pConn = ConnIt->second.pConnection;
      ConnIt->second.pConnection = nullptr;
      if(!m_Stopping) {
         m_Loop.addTimer(COLnewClosure0(&GRIDdeleteWebSocket, pConn), 0);
      } else {
         COL_TRC("Skipping timer add during shutdown for peer: " << IguanaId);
         delete pConn;
      }
   }
   COL_TRC("Cleaned up connection for peer: " << IguanaId);
}

void GRIDnode::onAuth(COLstring IguanaId) {
   COL_METHOD(GRIDnode::onAuth);
   auto ConnIt = m_Config.m_PeerMap.find(IguanaId);
   if(ConnIt != m_Config.m_PeerMap.end()) {
      GRIDclearConnectionError(&ConnIt->second);
      COL_TRC("Connection authenticated successfully, cleared any previous errors for " << IguanaId);
   }
}

void GRIDnode::setReadAccess(const COLstring& IguanaId, const GRIDreadAccessLevel Level) {
   COL_METHOD(GRIDnode::grantReadAccess);
   auto ConnIt = m_Config.m_PeerMap.find(IguanaId);
   if(ConnIt != m_Config.m_PeerMap.end()) { ConnIt->second.ReadAccess = Level; }
}

GRIDreadAccessLevel GRIDnode::getReadAccess(const COLstring& IguanaId) {
   COL_METHOD(GRIDnode::getReadAccess);
   auto ConnIt = m_Config.m_PeerMap.find(IguanaId);
   if(ConnIt != m_Config.m_PeerMap.end()) { return ConnIt->second.ReadAccess; }
   return GRIDreadAccessLevel::None;
}

// checks license for high availability and global dashboard
bool GRIDnode::validLicense() {
   COL_METHOD(GRIDnode::validLicense);
   COLvar LicenseVar;
   if(m_LicenseCallback) { m_LicenseCallback->run(&LicenseVar); }
   COL_VAR(LicenseVar);
   return LicenseVar["high_availability"].asBool() && LicenseVar["global_dashboard"].asBool();
}

void GRIDnode::updatePeerMetrics(const COLstring& IguanaId, const GRIDcompMetrics& Metrics) {
   COL_METHOD(GRIDnode::updatePeerMetrics);
   auto ConnIt = m_Config.m_PeerMap.find(IguanaId);
   if(ConnIt != m_Config.m_PeerMap.end()) {
      ConnIt->second.Metrics = Metrics;
      COL_TRC("Updated metrics for " << IguanaId << " CompsWithErr=" << Metrics.CompsWithErr << " MaxQueueCount="
                                     << Metrics.MaxQueueCount << " Throughput=" << Metrics.Throughput);
   } else {
      COL_TRC("Peer not found for metrics update: " << IguanaId);
   }
}

void GRIDnode::markPeerMetricsFailed(const COLstring& IguanaId) {
   COL_METHOD(GRIDnode::markPeerMetricsFailed);
   auto ConnIt = m_Config.m_PeerMap.find(IguanaId);
   if(ConnIt != m_Config.m_PeerMap.end()) {
      ConnIt->second.Metrics.CompsWithErr  = -1;
      ConnIt->second.Metrics.MaxQueueCount = -1;
      ConnIt->second.Metrics.Throughput    = -1;
      COL_TRC("Marked metrics failed for " << IguanaId);
   } else {
      COL_TRC("Peer not found for metrics failure: " << IguanaId);
   }
}

void GRIDnode::configure(const COLstring& IguanaId, GRIDwebSocket* pConnection) {
   COL_METHOD(GRIDnode::configure);
   pConnection->setOnHeartbeat(COLnewClosure1(this, &GRIDnode::onHeartbeat, IguanaId));
   pConnection->setOnClose(COLnewClosure0(this, &GRIDnode::onClose, IguanaId));
   pConnection->setOnAuth(COLnewClosure0(this, &GRIDnode::onAuth, IguanaId));
   pConnection->setOnJsonFrame(COLnewClosure1(&GRIDonJson, this, IguanaId, &m_Loop));
}
