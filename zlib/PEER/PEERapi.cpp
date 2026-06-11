//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: PEERapi.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   21/01/25 3:29 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <IWEB/IWEBconfig.h>
#include <PEER/PEERadd.h>
#include <PEER/PEERdelete.h>
#include <PEER/PEERapi.h>
#include <PEER/PEERpeers.h>
#include <PEER/PEERserverGuid.h>
#include <PEER/PEERstatus.h>
COL_LOG_MODULE;

void PEERapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap,
             PEERmap* pPeerMap, IWEBconfig* pWebConfig, SCKloop* pLoop) {
   COL_FUNCTION(PEERapi);
   // ApiMap.add("/peers",        COLnewClosure1(&PEERpeers, pPeerMap));
   // ApiMap.add("/peers/add",    COLnewClosure1(&PEERadd, pPeerMap, pLoop));
   // ApiMap.add("/peers/delete", COLnewClosure1(&PEERdelete, pPeerMap, pLoop));
   // ApiMap.add("/peers/status", COLnewClosure1(&PEERstatus, pPeerMap, pLoop));

   // ApiMap.add("/peers/server_guid", COLnewClosure1(&PEERserverGuid, pWebConfig));
}