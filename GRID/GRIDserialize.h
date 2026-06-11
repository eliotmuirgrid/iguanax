#ifndef _GRIDSERIALIZE_H
#define _GRIDSERIALIZE_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDserialize.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLhashmap.h>
#include <ctime>

class COLvar;
class GRIDwebSocket;
class IWEBconfig;
class TUNkey;
class SCKSsslContext;

enum class GRIDreadAccessLevel {
   None,
   Full
};

struct GRIDcompMetrics {
   int CompsWithErr  = 0;
   int MaxQueueCount = 0;
   int Throughput    = 0;
};

struct GRIDpeerInfo {
   GRIDwebSocket*      pConnection;
   COLstring           InstanceName;
   COLstring           Fingerprint;
   COLstring           Host;
   int                 Port;
   GRIDreadAccessLevel ReadAccess;
   GRIDcompMetrics     Metrics;

   COLstring LastError;
   int       LastErrorCode;
   time_t    LastErrorTime;

   GRIDpeerInfo() : pConnection(nullptr), Port(0), ReadAccess(GRIDreadAccessLevel::None), LastErrorCode(0), LastErrorTime(0) {}
   GRIDpeerInfo(const COLstring& H, const int P, const COLstring& K, const GRIDreadAccessLevel R=GRIDreadAccessLevel::None)
       : pConnection(nullptr), Fingerprint(K), Host(H), Port(P), ReadAccess(R), LastErrorCode(0), LastErrorTime(0) {}
};

struct GRIDnodeInfo {
   COLstring       m_IguanaId;
   bool            m_Enabled;
   IWEBconfig*     m_pWebConfig;
   TUNkey*         m_pPrivateKey;
   SCKSsslContext* m_pContext;
};

struct GRIDconfig {
   GRIDnodeInfo                        m_NodeInfo;
   COLhashmap<COLstring, GRIDpeerInfo> m_PeerMap;

   GRIDconfig() : m_NodeInfo() {}
};

COLstring GRIDreadAccessToString(GRIDreadAccessLevel Level);
GRIDreadAccessLevel GRIDreadAccessFromString(const COLstring& Str);
GRIDreadAccessLevel GRIDreadAccessFromBool(bool Value);

bool GRIDloadConfig(GRIDconfig* pConfig);

// removes instance config
bool GRIDupdateConfigRemove(const COLstring& PeerId, COLstring& Error);

// updates or create instance config
bool GRIDupdateConfigAdd(const COLstring& PeerId, const GRIDpeerInfo& Info, COLstring& Error);

#endif  // end of defensive include
