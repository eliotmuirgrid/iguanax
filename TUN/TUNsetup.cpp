// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNsetup
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thursday 15 June 2023 - 09:43AM
// ---------------------------------------------------------------------------

#include <TUN/TUNsetup.h>

#include <TUN/TUNrecordProcess.h>
#include <TUN/TUNmsgChallengeQuestion.h>
#include <TUN/TUNmsgChallengeResponse.h>
#include <TUN/TUNmsgChallengeAccepted.h>
#include <TUN/TUNmsgHello.h>
#include <TUN/TUNmsgPeerAsk.h>
#include <TUN/TUNmsgPeerAskClient.h>
#include <TUN/TUNmsgPeerAccept.h>
#include <TUN/TUNmsgPeerAcceptClient.h>
#include <TUN/TUNmsgPeerDisconnected.h>
#include <TUN/TUNmsgPeerData.h>
#include <TUN/TUNmsgPeerDataClient.h>


#include <TUN/TUNrecordType.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void TUNsetupClient(COLhashmap<COLuint32, COLauto<COLclosure1<TUNrequestItem*>>>* pHandlerTable, TUNstateClient* pState){
   COL_FUNCTION(TUNsetupClient);
   pHandlerTable->add(TUN_CHALLENGE_QUESTION, COLnewClosure1(&TUNmsgChallengeQuestion,     pState));
   pHandlerTable->add(TUN_CHALLENGE_ACCEPTED, COLnewClosure1(&TUNmsgChallengeAccepted,     pState));
   pHandlerTable->add(TUN_PEER_ASK,           COLnewClosure1(&TUNmsgPeerAskClient,         pState));
   pHandlerTable->add(TUN_PEER_ACCEPT,        COLnewClosure1(&TUNmsgPeerAcceptClient,      pState));
   pHandlerTable->add(TUN_PEER_DISCONNECTED,  COLnewClosure1(&TUNmsgPeerDisconnected,      pState));
   pHandlerTable->add(TUN_PEER_DATA,          COLnewClosure1(&TUNmsgPeerDataClient,        pState));
}

void TUNsetupServer(COLhashmap<COLuint32, COLauto<COLclosure1<TUNrequestItem*>>>* pHandlerTable, TUNstateServer* pState){
   COL_FUNCTION(TUNsetupServer);
   pHandlerTable->add(TUN_HELLO,              COLnewClosure1(&TUNmsgHello,             pState));
   pHandlerTable->add(TUN_CHALLENGE_RESPONSE, COLnewClosure1(&TUNmsgChallengeResponse, pState));
   pHandlerTable->add(TUN_PEER_ASK,           COLnewClosure1(&TUNmsgPeerAsk,           pState));
   pHandlerTable->add(TUN_PEER_ACCEPT,        COLnewClosure1(&TUNmsgPeerAccept,        pState));
   pHandlerTable->add(TUN_PEER_DATA,          COLnewClosure1(&TUNmsgPeerData,          pState));
}

// [client1] TUN_HELLO              --> [router]
// [client1] TUN_CHALLENGE_QUESTION <-- [router]
// [client1] TUN_CHALLENGE_RESPONSE --> [router]
// [client1] TUN_CHALLENGE_ACCEPTED <-- [router]
// [client1]                            [router] TUN_HELLO              <-- [ client2 ]
// [client1]                            [router] TUN_CHALLENGE_QUESTION --> [ client2 ]
// [client1]                            [router] TUN_CHALLENGE_RESPONSE <-- [ client2 ]
// [client1]                            [router] TUN_CHALLENGE_ACCEPTED --> [ client2 ]
// [client1] TUN_PEER_ASK           <-- [router] TUN_PEER_ASK           <-- [ client2 ]
// [client1] TUN_PEER_ACCEPT        --> [router] TUN_PEER_ACCEPT        --> [ client2 ]
// [client1] TUN_PEER_ASK           --> [router] TUN_PEER_ASK           --> [ client2 ]
// [client1] TUN_PEER_ACCEPT        <-- [router] TUN_PEER_ACCEPT        <-- [ client2 ]
