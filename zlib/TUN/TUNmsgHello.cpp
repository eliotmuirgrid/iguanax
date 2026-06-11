// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNmsgHello
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 14 June 2023 - 08:30AM
// ---------------------------------------------------------------------------

#include <TUN/TUNmsgHello.h>
#include <TUN/TUNrecord.h>
#include <TUN/TUNkeyConvert.h>
#include <TUN/TUNstateServer.h>
#include <TUN/TUNrecord.h>
#include <TUN/TUNrecordType.h>
#include <TUN/TUNkeyEllipticalGen.h>
#include <TUN/TUNchallenge.h>

#include <TUN/TUNstring.h>
#include <TUN/TUNrequest.h>

#include <SFI/SFIaesEncryptor.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// TUN_HELLO
void TUNmsgHello(TUNrequestItem* pRequest, TUNstateServer* pState){
   COL_FUNCTION(TUNmsgHello);
   COL_VAR(pRequest->ConnectionId);
   TUNconnection& Connection = pState->m_ConnectionMap[pRequest->ConnectionId];  // TODO should creation happen here?
   TUNcursor Cursor = TUNcursor(pRequest->RecordData);
   TUNstringRead(&Cursor, &Connection.m_Name);
   TUNstringRead(&Cursor, &Connection.m_ConnectionPublicKeyInDer);
   COL_DUMP("Client Key In DER:", Connection.m_ConnectionPublicKeyInDer.c_str(), Connection.m_ConnectionPublicKeyInDer.size());
   TUNkeyPublicFromDer(Connection.m_ConnectionPublicKeyInDer, &Connection.m_ConnectionPublicKey);
   TUNgenerateEllipticalKey(&Connection.m_TemporaryKey);
  
   COLstring EncryptedString;
   TUNchallenge(Connection.m_TemporaryKey, Connection.m_ConnectionPublicKey, &Connection.m_SharedSecret, &Connection.m_ChallengeString, &EncryptedString);
   
   COLstring Response;
   TUNstringWrite(&Response, TUNkeyPublicAsDer(Connection.m_TemporaryKey));
   TUNstringWrite(&Response, EncryptedString);
   TUNrecordWrite(pRequest, TUN_CHALLENGE_QUESTION, Response);  
}
