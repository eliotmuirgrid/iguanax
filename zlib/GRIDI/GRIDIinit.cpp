//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDinit.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <GRID/GRIDnode.h>
#include <GRID/GRIDserialize.h>
#include <GRIDI/GRIDIinit.h>
#include <GRIDS/GRIDSsecurity.h>
#include <IWEB/IWEBconfig.h>
#include <SCKS/SCKSsslContext.h>
#include <SCKS/SCKSsslUtils.h>
#include <TUN/TUNkey.h>
#include <TUN/TUNkeyFingerPrint.h>
COL_LOG_MODULE;

static void GRIDIsetupKey(GRIDconfig& Config) {
   COL_FUNCTION(GRIDIsetupKey);
   auto pPrivateKey = new TUNkey();
   GRIDSloadOrGenerateKey(pPrivateKey);
   const COLstring MyFingerprint = TUNkeyFingerPrint(*pPrivateKey);
   COL_TRC("GRID node key fingerprint: " << MyFingerprint);
   Config.m_NodeInfo.m_pPrivateKey = pPrivateKey;
}

static bool GRIDIsetupSslContext(GRIDconfig& Config) {
   COL_FUNCTION(GRIDIsetupSslContext);
   SCKSsslContext* pClientCtx = nullptr;
   if(Config.m_NodeInfo.m_pWebConfig->useHttps()) {
      pClientCtx = new SCKSsslContext(false);
      if(0 != SCKSsetClientCtx(*pClientCtx)) {
         COL_TRC("Failed to setup client SSL context: " << SCKSsslError());
         delete pClientCtx;
         delete Config.m_NodeInfo.m_pPrivateKey;
         return false;
      }
   }
   Config.m_NodeInfo.m_pContext = pClientCtx;
   return true;
}

static GRIDnode* GRIDIinitNode(GRIDconfig& Config, SCKloop& Loop, IWEBconfig* pWebConfig) {
   COL_FUNCTION(GRIDIinitNode);
   COL_TRC("Initializing GRID node: name=" << Config.m_NodeInfo.m_IguanaId << ", UseHttps=" << pWebConfig->useHttps());
   auto pNode = new GRIDnode(Config, Loop);
   pNode->startComponentPolling();
   return pNode;
}

static void GRIDIinitConnections(GRIDnode** ppNode, const GRIDconfig& Config) {
   COL_FUNCTION(GRIDIinitConnections);
   for(const auto& it : Config.m_PeerMap) {
      COL_VAR3(it.first, it.second.Host, it.second.Port);
      GRIDpeerInfo PeerInfo(it.second.Host, it.second.Port, it.second.Fingerprint, it.second.ReadAccess);
      COLstring    Err;
      if(!(*ppNode)->addPeer(it.first, PeerInfo, Err)) {
         COL_TRC("Failed to add peer " << it.first << ": " << Err);
      } else if(!(*ppNode)->connect(it.first)) {
         COL_TRC("Failed to connect to peer " << it.first);
      }
   }
}

bool GRIDIinit(const COLstring& IguanaId, IWEBconfig* pWebConfig, SCKloop* pLoop, GRIDnode** ppNode) {
   COL_FUNCTION(GRIDIinit);
   COL_VAR(IguanaId);
   GRIDconfig Config;
   Config.m_NodeInfo.m_Enabled    = true;
   Config.m_NodeInfo.m_IguanaId   = IguanaId;
   Config.m_NodeInfo.m_pWebConfig = pWebConfig;
   GRIDIsetupKey(Config);
   GRIDIsetupSslContext(Config);
   COL_TRC("Loading GRID config for instance: " << IguanaId << ", UseHttps: " << pWebConfig->useHttps());
   if(!GRIDloadConfig(&Config)) {
      COL_TRC("Failed to load GRID config");
      return false;
   }
   *ppNode = GRIDIinitNode(Config, *pLoop, pWebConfig);
   if(!*ppNode) {
      COL_TRC("Failed to initialize GRID node");
      return false;
   }
   GRIDIinitConnections(ppNode, Config);
   COL_TRC("GRID node initialized and configured successfully");
   return true;
}

void GRIDIreInit(GRIDnode* pNode) {
   COL_FUNCTION(GRIDIreInit);
   if(!pNode) {
      COL_TRC("GRID node is null, cannot reinitialize");
      return;
   }
   GRIDnodeInfo& NodeInfo = pNode->getNodeInfo();
   if(!NodeInfo.m_pWebConfig) {
      COL_TRC("GRID node has no web config, cannot reinitialize");
      return;
   }

   const bool UseHttps = NodeInfo.m_pWebConfig->useHttps();
   COL_TRC("Reinitializing GRID node: UseHttps=" << UseHttps);

   NodeInfo.m_pContext = nullptr;

   if(UseHttps) {
      auto pClientCtx = new SCKSsslContext(false);
      if(0 != SCKSsetClientCtx(*pClientCtx)) {
         COL_TRC("Failed to setup client SSL context: " << SCKSsslError());
         delete pClientCtx;
         return;
      }
      NodeInfo.m_pContext = pClientCtx;
   }
   COL_TRC("GRID node reinitialized successfully");
}
