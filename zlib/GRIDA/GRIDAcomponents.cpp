//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDAcomponents.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <GRID/GRIDrequest.h>
#include <CFG/CFGmap.h>
#include <COL/COLlog.h>
#include <COL/COLweb.h>
#include <GRID/GRIDnode.h>
#include <GRIDA/GRIDAcomponents.h>
#include <SDB/SDBcapi.h>
COL_LOG_MODULE;

void GRIDAcomponentsImpl(CFGmap* pComponentMap, COLvar* pOut) {
   COL_FUNCTION(GRIDAcomponentsImpl);
   int CompsWithErr  = 0;
   int MaxQueueCount = 0;
   int Throughput    = 0;
   for(const auto& it : pComponentMap->ConfigMap) {
      if(it.second.Status.ErrorCount > 0) { CompsWithErr++; }
      Throughput += SDBIgetComponentStats(it.second.CoreConfig.Guid, false);
      const int QueueCount = SDBIgetQueueCount(it.second.CoreConfig.Guid);
      if(QueueCount > MaxQueueCount) { MaxQueueCount = QueueCount; }
   }
   pOut->setMapType();
   (*pOut)["errors"]     = CompsWithErr;
   (*pOut)["queue"]      = MaxQueueCount;
   (*pOut)["throughput"] = Throughput;
   COL_VAR(pOut->json(1));
}

// /iguanas/components
void GRIDAcomponents(const COLwebRequest& Request, GRIDnode* pNode, CFGmap* pComponentMap) {
   COL_FUNCTION(GRIDAcomponents);
   COLvar Result;
   GRIDAcomponentsImpl(pComponentMap, &Result);
   COLrespondSuccess(Request.Address, Result);
}