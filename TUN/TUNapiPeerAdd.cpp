// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNapiPeerAdd
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Thursday 29 June 2023 - 02:22PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <SCK/SCKworker.h>
#include <TUN/TUNapiPeerAdd.h>
#include <TUN/TUNclientLoad.h>
#include <TUN/TUNhash.h>
#include <TUN/TUNkeyConvert.h>
#include <TUN/TUNsendPeerAsk.h>
#include <TUN/TUNstateClient.h>
COL_LOG_MODULE;

static void TUNpeerSaveCallback(bool Success, COLstring Error, COLaddress Address) {
   COL_FUNCTION(TUNpeerSaveCallback);
   COL_VAR2(Success, Error);
   Success ? COLrespondSuccess(Address) : COLrespondError(Address, Error);
}

bool TUNvalidName(const COLhashmap<COLstring, TUNpeer>& Peers,const COLstring& Name, COLstring* pOut){
   COL_FUNCTION(TUNvalidName);
   COLstring& Out = *pOut;
   if (Peers.count(Name)){
      Out = "Name of Peer already exists.";
      return false;
   }
   if (Name.is_null()){
      Out = "Name of Peer must be set.";
      return false;
   }
   if (Name.find(" ") != npos) {
      Out = "Name of Peer cannot contain spaces.";
      return false;
   }
   return true;
}

// /tunnel/peer/add
void TUNapiPeerAdd(const COLwebRequest& Request, TUNstateClient* pState){
   COL_FUNCTION(TUNapiPeerAdd);
   COLstring Name = COLvarRequiredString(Request.Data, "name"); 
   COLstring Key = COLvarRequiredString(Request.Data, "key");
   Name = Name.strip(COLstring::Trailing, ' ');
   Name = Name.strip(COLstring::Leading, ' ');
   COLstring Error;
   if(!TUNvalidName(pState->m_Peers, Name, &Error)){
      COLrespondError(Request.Address, Error);
      return;
   }
   TUNpeer& NewPeer = pState->m_Peers[Name];
   NewPeer.m_Label = Name;  // TODO redundant.
   try {
      TUNkeyPublicFromPemString(Key, &NewPeer.m_PublicKey);
   } catch(COLerror& Error){
      COL_TRC(Error.description());
      pState->m_Peers.removeKey(Name);
      COLrespondError(Request.Address, "Invalid key format.");
      return;
   }
   COLstring MyKey    = TUNkeyPublicAsPemString(pState->m_MyKey);
   COLstring KeyAdded = TUNkeyPublicAsPemString(NewPeer.m_PublicKey);
   if(MyKey.equals(KeyAdded)){
      pState->m_Peers.removeKey(Name);
      COLrespondError(Request.Address, "Key cannot be the same as your key.");
      return;
   }
   pState->m_KeyToPeerMap[TUNhashKey(NewPeer.m_PublicKey)] = Name;
   TUNsendPeerAddAsk(pState, Name);
   SCKwriteFile(pState->m_pLoop, DIRclusterDir(Name + ".pub"), Key, COLnewClosure2(TUNpeerSaveCallback, Request.Address));
}