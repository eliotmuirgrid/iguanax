// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNstateServer
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 14 June 2023 - 02:42PM
// ---------------------------------------------------------------------------

#include <TUN/TUNstateServer.h>
#include <TUN/TUNkeyFingerPrint.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

COLostream& operator<<(COLostream& Stream, const TUNconnection& Connection){
   Stream << "Conn " << TUNkeyFingerPrintFromDer(Connection.m_ConnectionPublicKeyInDer);
   return Stream;
}

COLostream& operator<<(COLostream& Stream, const TUNstateServer& Server){
   Stream << "ServerState:";
   Stream << newline << "Connection map (size=" << Server.m_ConnectionMap.size() << ")";
   for (auto i=Server.m_ConnectionMap.cbegin(); i != Server.m_ConnectionMap.cend(); i++){
      Stream << newline << i->first << " -> " << i->second;
   }
   Stream << newline << "Public Key map (" << Server.m_PublicKeyToIdMap.size() << ")";
   for (auto i=Server.m_PublicKeyToIdMap.cbegin(); i != Server.m_PublicKeyToIdMap.cend(); i++){
      Stream << newline << TUNkeyFingerPrintFromDer(i->first) << " -> " << i->second; 
   }
   return Stream;
}

void TUNconnectionName(COLuint64 ConnectionId, COLstring* pResult, TUNstateServer* pState){
   *pResult = pState->m_ConnectionMap[ConnectionId].m_Name;
}
