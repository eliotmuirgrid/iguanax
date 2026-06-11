// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNSserver
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Friday 16 June 2023 - 01:16PM
// ---------------------------------------------------------------------------

#include <TUNS/TUNSserver.h>

#include <TUN/TUNsetup.h>
#include <TUN/TUNstateServer.h>
#include <TUN/TUNrecordProcess.h>
#include <TUN/TUNrequest.h>
#include <TUN/TUNconnectionAddRemove.h>
#include <TUN/TUNrecordType.h>

#include <SCK/SCKloop.h>
#include <SCK/SCKlistener.h>
#include <SCK/SCKconnection.h>
#include <SCK/SCKconnectionRead.h>
#include <SCK/SCKconnectionWrite.h>

#include <COL/COLhashmap.h>
#include <COL/COLthreadPool.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

class TUNSserverState{
public:
   TUNstateServer m_State;
   COLhashmap<COLuint32, COLauto<COLclosure1<TUNrequestItem*>>> m_ServerHandlers;
};

void TUNSonData(COLuint64 ConnectionId, COLstring* pData, TUNSserverState* pState){
   COL_FUNCTION(TUNSonData);
   COL_VAR(ConnectionId);

   TUNrequest Request;
   Request.pIn = pData;
//   SCKconnectionRead(pState->m_pLoop, ConnectionId, &Request.pIn);
   COLstring Out;
   Request.pOut = &Out;
   COL_DUMP("Data: ", Request.pIn->c_str(), Request.pIn->size());
   Request.ConnectionId = ConnectionId;
   TUNrecordProcess(&Request, pState->m_ServerHandlers);
   SCKconnectionWrite(pState->m_State.m_pLoop, ConnectionId, Out);
}

void TUNShookup(SCKconnection* pConnection, TUNSserverState* pState){
   COL_FUNCTION(TUNShookup);
   pConnection->m_pOnData = COLnewClosure2(&TUNSonData, pState);
   pConnection->m_pOnClose = COLnewClosure1(&TUNconnectionRemove, &pState->m_State);
}

void TUNSaccept(SCKlistener* pListener, TUNSserverState* pState){
   COL_FUNCTION(TUNSaccept);
   SCKaddress Remote;
   int Socket = pListener->accept(&Remote);
   COL_VAR2(Socket, Remote);
   SCKconnection* pConnection = new SCKconnection(*pListener->loop(), Socket, 16);
   TUNShookup(pConnection, pState);
}

void TUNSserver(){
   COL_FUNCTION(TUNSserver);
   int Port = 443;
   COLcout << "Listening on port " << Port << newline;
   COLthreadPool Pool;
   SCKloop Loop(&Pool);

   TUNSserverState State;
   State.m_State.m_pLoop = &Loop;
   TUNsetNameMap(COLnewClosure2(&TUNconnectionName, &State.m_State));
   TUNsetupServer(&State.m_ServerHandlers, &State.m_State);
   SCKlistener Listener(Loop);
   Listener.setOnAccept(COLnewClosure0(&TUNSaccept, &Listener, &State));
   Listener.listen(SCKaddress(Port));
   Loop.start();
}
