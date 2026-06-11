//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDapiList.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <GRID/GRIDnode.h>
#include <GRID/GRIDsort.h>
#include <GRIDA/GRIDAlist.h>
COL_LOG_MODULE;

static void GRIDapiConnectionData(GRIDnode* pNode, COLvar& Out) {
   COL_FUNCTION(GRIDapiConnectionData);
   for(const auto& it : pNode->getPeers()) {
      const GRIDpeerInfo& Info         = it.second;
      bool                Https        = false;
      COLstring           InstanceName = Info.InstanceName;
      if(Info.pConnection) { Https = Info.pConnection->isSecure(); }
      COL_VAR(InstanceName);
      if(InstanceName.is_null()) { InstanceName = it.first; }
      COLvar Peer;
      Peer["id"]           = it.first;
      Peer["name"]         = InstanceName;
      Peer["host"]         = Info.Host;
      Peer["port"]         = Info.Port;
      Peer["https"]        = Https;
      Peer["fingerprint"]  = Info.Fingerprint;
      Peer["access_level"] = GRIDreadAccessToString(Info.ReadAccess);
      Peer["errors"]       = Info.Metrics.CompsWithErr;
      Peer["queue"]        = Info.Metrics.MaxQueueCount;
      Peer["throughput"]   = Info.Metrics.Throughput;
      Out.push_back(Peer);
   }
}

// /iguanas/list
void GRIDAlist(const COLwebRequest& Request, GRIDnode* pNode) {
   COL_FUNCTION(GRIDAlist);
   if(!pNode) { return COLrespondError(Request.Address, "Error getting server data."); }
   COLvar Response;
   Response.setArrayType();
   GRIDapiConnectionData(pNode, Response);
   const COLstring SortBy    = COLvarGetString(Request.Data, "sort_by", "status");
   const bool      Ascending = COLvarGetBool(Request.Data, "ascending", false);
   GRIDsortPeers(&Response, pNode, SortBy, Ascending);
   COL_VAR(Response.json(1));
   COLrespondSuccess(Request.Address, Response);
}