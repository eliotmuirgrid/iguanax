#ifndef __TUN_SETUP_H__
#define __TUN_SETUP_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNsetup
//
// Description:
//
// Set up the record handler states for both server and client
//
// Author: Eliot Muir
// Date:   Thursday 15 June 2023 - 09:43AM
// ---------------------------------------------------------------------------

#include <COL/COLhashmap.h>
#include <COL/COLclosure.h>
#include <COL/COLauto.h>

class COLstring;
class TUNstateClient;
class TUNstateServer;
class TUNrequestItem;

void TUNsetupClient(COLhashmap<COLuint32, COLauto<COLclosure1<TUNrequestItem*>>>* pHandlerTable, TUNstateClient* pState);
void TUNsetupServer(COLhashmap<COLuint32, COLauto<COLclosure1<TUNrequestItem*>>>* pHandlerTable, TUNstateServer* pState);

#endif // end of defensive include
