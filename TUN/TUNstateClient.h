#ifndef __TUN_STATE_CLIENT_H__
#define __TUN_STATE_CLIENT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNstateClient
//
// Description:
//
// State of a client connection
//
// Author: Eliot Muir
// Date:   Wednesday 14 June 2023 - 03:13PM
// ---------------------------------------------------------------------------

#include <TUN/TUNkey.h>
#include <TUN/TUNsetup.h>
#include <TUN/TUNconnectionRefresh.h>

#include <COL/COLstring.h>
#include <COL/COLhashmap.h>
#include <COL/COLauto.h>
#include <COL/COLclosure.h>

class SCKloop;
class TUNrequestInProgress;

class TUNpeer{
public:
   TUNkey    m_PublicKey;
   COLstring m_Md5Key;
   COLstring m_Label;
   // COLstring m_Description
   COLstring m_SharedSecret;
   COLstring m_Challenge;

   COLstring m_BufferOut;
   COLstring m_BufferIn;
   bool m_Authenticated=false;
   int  m_PeerProtocolVer = 0;
};

class TUNstateClient{
public:
   TUNstateClient();
   ~TUNstateClient();
   TUNkey m_MyKey;
   COLstring m_Name;
   SCKloop* m_pLoop;
   COLuint64 m_ConnectionId=0;

   COLhashmap<COLstring, TUNpeer> m_Peers;
   COLhashmap<COLstring, COLstring> m_KeyToPeerMap;

   COLauto<COLclosure2< const COLstring&, const COLstring&> > m_pOnData;
   COLauto<COLclosure1< const COLstring& > > m_pOnConnect;
   COLauto<COLclosure1< const COLstring& > > m_pOnDisconnect;

   COLauto<COLclosure0> m_pOnRouterConnect;
   COLauto<COLclosure0> m_pOnRouterDisconnect;

   COLhashmap<COLuint32, COLauto<COLclosure1<TUNrequestItem*>> > m_HandlerTable;

   COLhashmap<COLuint64, COLclosure1<const COLvar&>* > m_LuaConnections;

   COLuint64                                           m_RequestId = 0;
   COLhashmap<COLstring, TUNrequestInProgress*>        m_RequestsInProgress;

   int    m_ReconnectTimerId = 0;
   int    m_ConnectionRefreshTimerId = 0;
   time_t m_LastActivity;

   bool m_Respond = true;
};

COLostream& operator<<(COLostream& Stream, const TUNstateClient& Client);

#endif // end of defensive include~
