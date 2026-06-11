#ifndef __TUN_STATE_SERVER_H__
#define __TUN_STATE_SERVER_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNstateServer
//
// Description:
//
// State for a server tunnel connection.
//
// Author: Eliot Muir 
// Date:   Wednesday 14 June 2023 - 02:42PM
// ---------------------------------------------------------------------------

#include <TUN/TUNkey.h>

#include <COL/COLstring.h>
#include <COL/COLhashmap.h>

class SCKloop;

class TUNconnectionPeer{
public:
   COLuint64 m_ConnectionId; 
   bool      m_Verified=false;  
};

class TUNconnection{
public:
   TUNkey m_TemporaryKey;
   TUNkey m_ConnectionPublicKey;
   COLstring m_ConnectionPublicKeyInDer;
   COLstring m_ChallengeString;
   COLstring m_SharedSecret;
   COLstring m_Name;  // NOT GUARANTEED
   COLhashmap<COLstring, TUNconnectionPeer> m_PeerMap;
   COLhashmap<COLuint64, COLstring> m_ConnectionIdToPeerMap;
};

COLostream& operator<<(COLostream& Stream, const TUNconnection& Connection);

class TUNstateServer{
public:
   COLhashmap<COLuint64, TUNconnection> m_ConnectionMap;
   COLhashmap<COLstring, COLuint64> m_PublicKeyToIdMap;
   SCKloop* m_pLoop;
};

void TUNconnectionName(COLuint64 ConnectionIndex, COLstring* pResult, TUNstateServer* pState);

COLostream& operator<<(COLostream& Stream, const TUNstateServer& Server);

#endif // end of defensive include
