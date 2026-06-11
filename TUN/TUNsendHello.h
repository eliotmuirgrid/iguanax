#ifndef __TUN_SEND_HELLO_H__
#define __TUN_SEND_HELLO_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNsendHello
//
// Description:
//
// We start by saying hello
//
// Author: Eliot Muir 
// Date:   Friday 23 June 2023 - 05:26PM
// ---------------------------------------------------------------------------

class COLstring;
class TUNstateClient;

void TUNsendHello(COLstring* pSendBuffer, const TUNstateClient& State);

#endif // end of defensive include
