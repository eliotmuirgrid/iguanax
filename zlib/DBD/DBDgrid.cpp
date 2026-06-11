//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: DBDgrid.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLclosure.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <DBD/DBDgrid.h>
#include <DRX/DRXiguanaId.h>
#include <DRX/DRXlicenseCheck.h>
#include <GRIDI/GRIDIinit.h>
#include <GRIDI/GRIDIupgrade.h>
#include <GRID/GRIDnode.h>
#include <WEB/WEBserver.h>
COL_LOG_MODULE;

static void DBDreinitGrid(GRIDnode** ppGridNode, WEBserver* pWebServer, SCKloop* pLoop) {
   COL_FUNCTION(DBDreinitGrid);
   pWebServer->setOnWebSocketUpgrade(nullptr);
   GRIDIreInit(*ppGridNode);
   pWebServer->setOnWebSocketUpgrade(COLnewClosure1(&GRIDIprocessWsUpgradeRequest, *ppGridNode, pLoop));
}

void DBDgridInit(WEBserver* pWebServer, IWEBconfig* pWebConfig, GRIDnode** ppGridNode, SCKloop* pLoop) {
   COL_FUNCTION(DBDgridInit);
   if(GRIDIinit(DRXiguanaId(), pWebConfig, pLoop, ppGridNode)) {
      COL_TRC("GRID node initialized, registering WebSocket handler and license callback");
      pWebServer->setOnWebSocketUpgrade(COLnewClosure1(&GRIDIprocessWsUpgradeRequest, *ppGridNode, pLoop));
      pWebServer->setOnRestartGrid(COLnewClosure0(&DBDreinitGrid, ppGridNode, pWebServer, pLoop));
      (*ppGridNode)->setLicenseCallback(COLnewClosure1<COLvar*>(&DRXlicenseEntitlement));
   } else {
      COL_TRC("GRID initialization failed, not registering handler");
   }
}
