// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LTUNconnectionId
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Monday 10 July 2023 - 11:17AM
// ---------------------------------------------------------------------------

#include <LTUN/LTUNconnectionId.h>

#include <LUA/LUAutils.h>

#include <SCK/SCKloop.h>

#include <COL/COLevent.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void LTUNgetIdMain(COLuint64* pConnectionId, SCKloop* pLoop, COLevent* pOperationDone){
   COL_FUNCTION(LTUNgetIdMain);
   *pConnectionId = pLoop->m_NextSocketId;
   COL_TRC("Got translator connection id: " << *pConnectionId << " , incrementing SCKloop's m_NextSocketId.");
   pLoop->m_NextSocketId++; // TODO - is this sufficient, or do we need a way to tell SCK this one is now in use
   pOperationDone->signal();
}

void LTUNsetConnectionId(lua_State* L, SCKloop* pLoop){
   COL_FUNCTION(LTUNsetConnectionId);
   COLuint64 ConnectionId;
   
   COLevent GotId;
   pLoop->post(COLnewClosure0(&LTUNgetIdMain, &ConnectionId, pLoop, &GotId));
   GotId.wait();

   COL_TRC("Connection id received from main thread, set in lua state.");
   LUAsetPointerField(L, "_pConnectionId", (void*)ConnectionId);
}