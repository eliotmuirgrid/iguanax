//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: PEERdelete.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   22/01/25 12:14 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <PEER/PEERdelete.h>
#include <SCK/SCKconnection.h>
COL_LOG_MODULE;

void PEERdelete(const COLwebRequest& Request, PEERmap* pPeerMap, SCKloop* pLoop) {
   COL_FUNCTION(PEERdelete);
   const COLstring Peer = COLvarRequiredString(Request.Data, "peer");
   if(pPeerMap->m_PeerMap.count(Peer) != 0)       { pPeerMap->m_PeerMap.removeKey(Peer); }
   if(pPeerMap->m_ConnectionMap.count(Peer) != 0) { pPeerMap->m_ConnectionMap[Peer]->pConnection->close(); }
   COLrespondSuccess(Request.Address);
}