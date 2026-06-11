// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNmsgChallengeQuestion
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 14 June 2023 - 04:46PM
// ---------------------------------------------------------------------------

#include <TUN/TUNmsgChallengeQuestion.h>
#include <TUN/TUNstring.h>
#include <TUN/TUNkey.h>
#include <TUN/TUNkeyConvert.h>
#include <TUN/TUNstateClient.h>
#include <TUN/TUNrecord.h>
#include <TUN/TUNrequest.h>
#include <TUN/TUNrecordType.h>
#include <TUN/TUNchallenge.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// TUN_CHALLENGE_QUESTION - back to client
void TUNmsgChallengeQuestion(TUNrequestItem* pRequest, TUNstateClient* pState){
   COL_FUNCTION(TUNmsgChallengeQuestion);
   COL_DUMP("RecordData:", pRequest->RecordData.c_str(), pRequest->RecordData.size());  
   COLstring PublicPeerDerKey;
   COLstring EncryptedString;

   TUNcursor Cursor(pRequest->RecordData);
   // TODO we should close the connection if TUNstringRead returns false
   TUNstringRead(&Cursor, &PublicPeerDerKey);
   TUNstringRead(&Cursor, &EncryptedString);
   COL_DUMP("PublicPeerDerKey:", PublicPeerDerKey.c_str(), PublicPeerDerKey.size());  
   COL_DUMP("EncryptedString:", EncryptedString.c_str(), EncryptedString.size());  
   TUNkey TemporaryServerKey;
   TUNkeyPublicFromDer(PublicPeerDerKey, &TemporaryServerKey);

   TUNrecordWrite(pRequest,TUN_CHALLENGE_RESPONSE, TUNchallengeResponse(pState->m_MyKey, TemporaryServerKey, EncryptedString));
   TUNupdateLastActionTime(&pState->m_LastActivity);
}
