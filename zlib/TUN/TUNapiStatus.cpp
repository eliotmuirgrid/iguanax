// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNapiStatus
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Thursday 29 June 2023 - 11:20AM
// ---------------------------------------------------------------------------

#include <TUN/TUNapiStatus.h>
#include <TUN/TUNstateClient.h>

#include <COL/COLweb.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// /tunnel/status
void TUNapiStatus(const COLwebRequest& Request, TUNstateClient* pState) {
   COL_FUNCTION(TUNapiStatus);
   COLvar Response;
   Response["connected"] = (pState->m_ConnectionId != 0);
   COLvar& Peers= Response["peers"].setMapType();
   for (auto i=pState->m_Peers.begin(); i != pState->m_Peers.end(); i++) {
      Peers[i->first]["authenticated"] = i->second.m_Authenticated;
      Peers[i->first]["hash"]          = i->second.m_Md5Key;
   }
   COLrespondSuccess(Request.Address, Response);
}