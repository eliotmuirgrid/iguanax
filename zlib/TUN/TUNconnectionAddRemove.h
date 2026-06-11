#ifndef __TUN_CONNECTION_ADD_REMOVE_H__
#define __TUN_CONNECTION_ADD_REMOVE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNconnectionAddRemove
//
// Description:
//
// We call this routine to clean up an connection that is disconnected.
//
// Author: Eliot Muir 
// Date:   Friday 23 June 2023 - 04:39PM
// ---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
class TUNstateServer;


// Only gets called when we have authenticated the connection
void TUNconnectionRegister(COLuint64 ConnectionId, TUNstateServer* pState);

void TUNconnectionRemove(COLuint64 ConnectionId, TUNstateServer* pState);



#endif // end of defensive include
