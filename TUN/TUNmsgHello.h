#ifndef __TUN_MSG_HELLO_H__
#define __TUN_MSG_HELLO_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNmsgHello
//
// Description:
//
// Routine to format hello greeting.
//
// Author: Eliot Muir 
// Date:   Wednesday 14 June 2023 - 08:30AM
// ---------------------------------------------------------------------------

class TUNstateServer;
class TUNrequestItem;

void TUNmsgHello(TUNrequestItem* pRequest, TUNstateServer* pState);


#endif // end of defensive include
