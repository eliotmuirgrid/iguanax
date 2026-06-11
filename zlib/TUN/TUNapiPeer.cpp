// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNapiPeer
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Thursday 29 June 2023 - 02:49PM
// ---------------------------------------------------------------------------

#include <TUN/TUNapiPeer.h>
#include <TUN/TUNstateClient.h>
#include <TUN/TUNkeyConvert.h>


#include <COL/COLweb.h>
#include <COL/COLvarUtils.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// /tunnel/peer
void TUNapiPeer(const COLwebRequest& Request, TUNstateClient* pState){
   COL_FUNCTION(TUNapiPeer);
   COLstring Name = COLvarRequiredString(Request.Data, "name"); 

   if (pState->m_Peers.count(Name) == 0){
      COLrespondError(Request.Address, "Did not find peer with that name.");
      return;
   }
   TUNpeer& Peer = pState->m_Peers[Name];
   COLvar Result;
   // TODO - what if the next function throws
   Result["key"] = TUNkeyPublicAsPemString(Peer.m_PublicKey);
   Result["name"] = Peer.m_Label;
   Result["connected"] = Peer.m_Authenticated;
   COLrespondSuccess(Request.Address, Result);
} 