// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNmsgPeerData
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 28 June 2023 - 01:27PM
// ---------------------------------------------------------------------------

#include <TUN/TUNmsgPeerData.h>
#include <TUN/TUNstring.h>
#include <TUN/TUNrequest.h>
#include <TUN/TUNstateServer.h>
#include <TUN/TUNnumber.h>
#include <TUN/TUNrecord.h>
#include <TUN/TUNrecordType.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void TUNmsgPeerData(TUNrequestItem* pRequest, TUNstateServer* pState){
   COL_FUNCTION(TUNmsgPeerData);
   TUNcursor Cursor(pRequest->RecordData);
   COLstring PeerIndex;
   TUNstringRead(&Cursor, &PeerIndex);
   COLstring Data;
   TUNstringRead(&Cursor, &Data);
   COL_VAR2(PeerIndex, Data);
   TUNconnection& Conn = pState->m_ConnectionMap[pRequest->ConnectionId];
   if (Conn.m_PeerMap.count(PeerIndex) == 0){
      COL_ERR("No such peer index");
      return;
   }
   COLuint64 ConnectionId = Conn.m_PeerMap[PeerIndex].m_ConnectionId;   // TODO verify that it's authenticated.
   COL_VAR(ConnectionId);
   COLstring Out;
   PeerIndex = pState->m_ConnectionMap[ConnectionId].m_ConnectionIdToPeerMap[pRequest->ConnectionId];
   TUNstringWrite(&Out, PeerIndex);  
   TUNstringWrite(&Out, Data);
   TUNrecordConnectionWrite(pState->m_pLoop, ConnectionId, TUN_PEER_DATA, Out);
}