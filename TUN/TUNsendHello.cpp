// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNsendHello
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Friday 23 June 2023 - 05:26PM
// ---------------------------------------------------------------------------

#include <TUN/TUNsendHello.h>
#include <TUN/TUNrecordType.h>
#include <TUN/TUNrecord.h>
#include <TUN/TUNkeyConvert.h>
#include <TUN/TUNstateClient.h>
#include <TUN/TUNstring.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void TUNsendHello(COLstring* pSendBuffer, const TUNstateClient& State){
   COL_FUNCTION(TUNsendHello);
   COLstring Data;
   COLstring PublicKey = TUNkeyPublicAsDer(State.m_MyKey);
   TUNstringWrite(&Data, State.m_Name);
   TUNstringWrite(&Data, PublicKey);
   TUNrecordOutTrace(TUN_HELLO, 0);
   TUNrecordWriteString(pSendBuffer, TUN_HELLO, Data);
}
