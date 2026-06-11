//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDAstatus.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <GRIDA/GRIDAstatus.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <GRID/GRIDnode.h>
COL_LOG_MODULE;

static void GRIDAstatusPeer(const GRIDpeerInfo& Info, COLvar& Output) {
   COL_FUNCTION(GRIDAstatusPeer);
   const bool Connected = Info.pConnection && Info.pConnection->connected();
   Output["connected"]  = Connected;
   Output["latency_ms"] = Connected ? COLint64ToString(Info.pConnection->getLastRtt()) : "";
   Output["error_code"] = Info.LastErrorCode;
   Output["error_msg"]  = Info.LastError;
   Output["error_time"] = COLint64ToString(Info.LastErrorTime);
}

// iguanas/status
void GRIDAstatus(const COLwebRequest& Request, GRIDnode* pNode) {
   COL_FUNCTION(GRIDAstatus);
   COLvar Res;
   for(const auto& it : pNode->getPeers()) {
      COL_VAR(it.first);
      GRIDAstatusPeer(it.second, Res[it.first]);
   }
   COL_VAR(Res.json(1));
   COLrespondSuccess(Request.Address, Res);
}