// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNmsgChallengeAccepted
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Thursday 15 June 2023 - 12:03PM
// ---------------------------------------------------------------------------

#include <TUN/TUNmsgChallengeAccepted.h>
#include <TUN/TUNstateClient.h>
#include <TUN/TUNrequest.h>
#include <TUN/TUNsendPeerAsk.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// TUN_CHALLENGE_ACCEPTED
void TUNmsgChallengeAccepted(TUNrequestItem* pRequest, TUNstateClient* pState){
   COL_FUNCTION(TUNmsgChallengeAccepted);
   COL_VAR(pRequest->RecordData);
   if (pRequest->RecordData == "TRUE"){  // TODO we could use a number?
      COL_TRC("We are authenticated with the router.  Send peer keys");
      TUNsendAllPeerAsk(pState, pRequest);
   }
   TUNupdateLastActionTime(&pState->m_LastActivity);
}

// TODO - we must encrypt the connection with the shared secret after this.