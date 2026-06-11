// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNmsgChallengeResponse
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 14 June 2023 - 05:26PM
// ---------------------------------------------------------------------------

#include <TUN/TUNmsgChallengeResponse.h>
#include <TUN/TUNstateServer.h>
#include <TUN/TUNrecord.h>
#include <TUN/TUNsetup.h>
#include <TUN/TUNrequest.h>
#include <TUN/TUNstateServer.h>
#include <TUN/TUNkeyConvert.h>
#include <TUN/TUNconnectionAddRemove.h>
#include <TUN/TUNrecordType.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// TUN_CHALLENGE_RESPONSE
void TUNmsgChallengeResponse(TUNrequestItem* pRequest, TUNstateServer* pState){
   COL_FUNCTION(TUNmsgChallengeResponse);
   COL_DUMP("Returned:", pRequest->RecordData.c_str(), pRequest->RecordData.size());
   TUNconnection& Connection = pState->m_ConnectionMap[pRequest->ConnectionId];
   COL_DUMP("Expected:", Connection.m_ChallengeString.c_str(), Connection.m_ChallengeString.size());   
   TUNconnectionRegister(pRequest->ConnectionId, pState);
   
   if (pRequest->RecordData == Connection.m_ChallengeString){
      COL_TRC("Challenge accepted!");
      TUNrecordWrite(pRequest, TUN_CHALLENGE_ACCEPTED, "TRUE");
   } else {
      COL_TRC("Challenge failed.  You don't own this key.");
      TUNrecordWrite(pRequest, TUN_CHALLENGE_ACCEPTED, "FALSE");
   }
   COL_VAR(*pState);
}
