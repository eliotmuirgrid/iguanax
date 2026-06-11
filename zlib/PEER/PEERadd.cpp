//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: PEERadd.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   21/01/25 3:29 PM
//  ---------------------------------------------------------------------------
#include <COL/COLclosure.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <PEER/PEERadd.h>
#include <PEER/PEERmap.h>
#include <PEER/PEERsocket.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

// /peers/add
void PEERadd(const COLwebRequest& Request, PEERmap* pPeerMap, SCKloop* pLoop) {
   COL_FUNCTION(PEERadd);
   COL_VAR(Request.Data);
   const COLstring HostAddress = COLvarRequiredString(Request.Data, "address");
   if(PEERmapAddressExists(*pPeerMap, HostAddress)) {
      return COLrespondError(Request.Address, HostAddress + " already exists in map.");
   }
   auto* pNode = new PEERnode();
   if(PEERsocketInit(pPeerMap, pNode, HostAddress, pLoop)) {

      return COLrespondSuccess(Request.Address);
   }
   COLrespondError(Request.Address, "Failed to connect to " + HostAddress);
}