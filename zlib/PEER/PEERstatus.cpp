//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: PEERstatus.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   23/01/25 11:57 AM
//  ---------------------------------------------------------------------------
#include <PEER/PEERstatus.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
COL_LOG_MODULE;

void PEERstatus(const COLwebRequest& Request, PEERmap*  pPeerMap, SCKloop* pLoop) {
   COL_FUNCTION(PEERstatus);
   COLvar StatusMap;
   for(const auto& it : pPeerMap->m_PeerMap) {
      COL_VAR(it.first);
      StatusMap[it.first] = pPeerMap->m_ConnectionMap.count(it.first) != 0;
   }
   COL_VAR(StatusMap.json(1));
   COLrespondSuccess(Request.Address, StatusMap);
}