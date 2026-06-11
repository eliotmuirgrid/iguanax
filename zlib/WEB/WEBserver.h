#ifndef __WEB_SERVER_H__
#define __WEB_SERVER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: WEB2server
//
// Author: Eliot Muir
//
// Description:
//
// This is a web server built on the SCK library and HTTP libraries.
//---------------------------------------------------------------------------

#include <SCK/SCKlistener.h>
#include <SCK/SCKloop.h>
#include <SCKS/SCKSsslContext.h>

#include <COL/COLarray.h>
#include <COL/COLweb.h>

#include <WEB/WEBsession.h>
#include <WEB/WEBrequestData.h>

class SCKSconnection;
class SCKaddress;
class WEBconnection;
class WEBrequestData;
class IWEBconfig;

// Why don't we use the socket handle as the key for connections?  The reason is that socket's can be reused - i.e. close socket 56 and create a new socket and you get 56.
// It would be highly confusing if a thread were to write to that socket.
class WEBconnection{
public:
   WEBconnection(const SCKaddress& Address);
   ~WEBconnection();

   SCKaddress     m_Address;
   WEBrequestData m_RequestData;
   time_t         m_LastActive;
   bool           m_IsWs;
};

class WEBserver{
public:
   WEBserver(SCKloop& Loop);
   ~WEBserver();

   bool start(int Port, const COLstring& BindIp="", bool IsSecure=false);
   int  port() { return m_pListener->port(); }
   void shutdown();
   bool write(COLuint64 ConnectionId, const char* pData, int Size);    // This method gives us the simplest interface to send a reponse.  An HTTP response is just writing data back to the right socket.

   const SCKaddress& address(COLuint64 Id) const;
   int lastError() const;
   void onRawData(const COLstring* pData, SCKconnection* pConnection);
   void onData (COLuint64 ConnectionId, COLstring* pIn);
   void onClose(COLuint64 ConnectionId);
   void onAccept();
   void isWebSocket(COLuint64 ConnectionId);
   void setOnWebSocketUpgrade(COLauto<COLclosure1<const WEBrequestData&>> pHandler);
   void setOnRestartGrid(COLauto<COLclosure0> pHandler);

   bool m_RequireAuthentication;
   COLhashmap<COLstring, COLwebHandlerFunc>             m_WorkerThreadHandlers;   // Handled on worker threads.
   COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> > m_MainThreadHandlers;     // Handled on main thread

   COLauto<SCKlistener> m_pListener;
   SCKloop* m_pLoop;
   SCKSsslContext m_SslContext;
   SCKSsslContext* m_pCtx;

   int m_ConnectionTimerId;
   int m_SessionTimerId;
   COLhashmap<COLuint64, WEBconnection*> m_Connections;
   COLmap<COLstring, WEBsession> m_SessionMap;
   COLstring m_IndexPage { "index.html" };

   // COLauto< COLclosure2<const WEBrequestData&, bool*> > m_pWillSpecialHandle;
   // COLauto< COLclosure1<const WEBrequestData&> > m_pSpecialHandler;

   COLauto< COLclosure1<const WEBrequestData&> > m_pSamlPostHandler;
   COLauto<COLclosure1<const WEBrequestData&>> m_pWebSocketUpgradeHandler;
   COLauto<COLclosure0> m_pOnRestartGrid;

   WEBserver(const WEBserver& Orig);
   WEBserver& operator=(const WEBserver& Orig);
};

void WEBserverSetResponseCompleteCallback(COLclosure0* pCallback);

void WEBserverRunResponseCompleteCallback();

void WEBserverClearHandlers(WEBserver* pServer);

#endif // end of defensive include
