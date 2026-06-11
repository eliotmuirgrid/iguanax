// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testTUNclientLoad
//
// Description:
//
// Implementation of tests
//
// Author: Eliot Muir 
// Date:   Monday 26 June 2023 - 02:11PM
// ---------------------------------------------------------------------------
#include "testTUNclientLoad.h"

#include <TUN/TUNstateClient.h>
#include <TUN/TUNclientLoad.h>

#include <UNIT/UNITapp.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testLoad(){
   COL_FUNCTION(testLoad);
   TUNstateClient State;
   TUNclientLoad("../TUNclient/client/client1.config", &State);

   COL_VAR(State);
}

void testTUNclientLoad(UNITapp* pApp) {
   pApp->add("load/simple", &testLoad);
}
