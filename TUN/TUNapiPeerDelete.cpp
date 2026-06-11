// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNapiPeerDelete
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Thursday 29 June 2023 - 02:39PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <SCK/SCKworker.h>
#include <TUN/TUNapiPeerDelete.h>
#include <TUN/TUNhash.h>
#include <TUN/TUNstateClient.h>
COL_LOG_MODULE;

static void TUNdeletePeerPemCallback(COLvar Result, COLaddress Address) {
   COL_FUNCTION(TUNdeletePeerPemCallback);
   Result["success"].asBool() ? COLrespondSuccess(Address) : COLrespondError(Address, Result["error"]);
}

static void TUNdeletePeerPemWorker(const COLvar& Data, COLvar* pOut) {
   COL_FUNCTION(TUNdeletePeerPemWorker);
   const COLstring Name    = Data["name"];
   const COLstring PemFile = DIRclusterDir(Name + ".pem");
   try {
      if(FILfileExists(PemFile)) {
         FILremove(PemFile, true);
      } else {
         COL_TRC(PemFile + " not found");
      }
      (*pOut)["success"] = true;
   } catch(const COLerror& E) {
      COL_TRC(E.description());
      (*pOut)["success"] = false;
      (*pOut)["error"]   = E.description();
   }
}

// /tunnel/peer/delete
void TUNapiPeerDelete(const COLwebRequest& Request, TUNstateClient* pState){
   COL_FUNCTION(TUNapiPeerDelete);
   COLstring Name = COLvarRequiredString(Request.Data, "name");
   if (pState->m_Peers.count(Name) == 0){
      return COLrespondError(Request.Address, "Did not find peer with that name.");
   }
   pState->m_KeyToPeerMap.removeKey(TUNhashKey(pState->m_Peers[Name].m_PublicKey));
   pState->m_Peers.removeKey(Name);

   // TODO we could tell the router we are not friends any more.
   SCKinvokeWorker(pState->m_pLoop, TUNdeletePeerPemWorker, Request.Data, COLnewClosure1(TUNdeletePeerPemCallback, Request.Address));
}