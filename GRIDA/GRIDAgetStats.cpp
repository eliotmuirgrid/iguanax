//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDapiGetStats.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLweb.h>
#include <COL/COLvarUtils.h>
#include <GRID/GRIDnode.h>
#include <GRIDA/GRIDAgetStats.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

static void GRIDgetCurrentStats(GRIDnode* pNode, const COLaddress Address) {
   COL_FUNCTION(GRIDgetRealTimeStats);
   COLvar Data;
   pNode->getCurrentStats(&Data) ? COLrespondSuccess(Address, Data) : COLrespondError(Address, Data);
}

static void GRIDgetStatsDisabled(const COLwebRequest& Request) {
   COL_FUNCTION(GRIDgetStatsDisabled);
   COLvar Data;
   COLvar Points;
   Points.setArrayType();
   // Keep the response shape stable for callers while historical
   // resource storage and visualization are turned off for this release.
   Data["metric"]   = COLvarGetString(Request.Data, "metric", "");
   Data["from"]     = COLvarGetString(Request.Data, "from", "");
   Data["to"]       = COLvarGetString(Request.Data, "to", "");
   Data["points"]   = Points;
   Data["disabled"] = true;
   Data["message"]  = "Resource history is disabled.";
   COLrespondSuccess(Request.Address, Data);
}

// /resources/current
void GRIDAgetCurrentStats(const COLwebRequest& Request, GRIDnode* pNode, SCKloop* pLoop) {
   COL_FUNCTION(GRIDAgetCurrentStats);
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&GRIDgetCurrentStats, pNode, Request.Address));
}

// /resources/history
void GRIDAgetStatHistory(const COLwebRequest& Request, GRIDnode* pNode, SCKloop* pLoop) {
   COL_FUNCTION(GRIDAgetStatHistory);
   (void)pNode;
   (void)pLoop;
   GRIDgetStatsDisabled(Request);
}
