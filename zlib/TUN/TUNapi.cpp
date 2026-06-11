// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNapi
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thursday 29 June 2023 - 11:17AM
// ---------------------------------------------------------------------------

#include <TUN/TUNapi.h>
#include <TUN/TUNapiStatus.h>
#include <TUN/TUNapiKeyPublic.h>
#include <TUN/TUNapiPeerAdd.h>
#include <TUN/TUNapiPeerDelete.h>
#include <TUN/TUNapiPeer.h>
#include <TUN/TUNstopListening.h>
#include <TUN/TUNkeyRegen.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void TUNapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, TUNstateClient* pClient){
   // IX-3790 - turned off clustering
   // ApiMap.add("/tunnel/status"     , COLnewClosure1(&TUNapiStatus,    pClient));
   // ApiMap.add("/tunnel/key/public" , COLnewClosure1(&TUNapiKeyPublic, pClient));
   // ApiMap.add("/tunnel/peer/add"   , COLnewClosure1(&TUNapiPeerAdd,   pClient));
   // ApiMap.add("/tunnel/peer/delete", COLnewClosure1(&TUNapiPeerDelete,pClient));
   // ApiMap.add("/tunnel/peer",        COLnewClosure1(&TUNapiPeer,      pClient));
   // ApiMap.add("/tunnel/stop",        COLnewClosure1(&TUNstopListening, pClient));
   // ApiMap.add("/tunnel/key/regen",   COLnewClosure1(&TUNkeyRegen, pClient));
}