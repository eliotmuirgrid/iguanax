#ifndef __SCK_LISTENER_H__
#define __SCK_LISTENER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: SCKlistener
//
// Author: Eliot Muir
//
// Description:
//
// Wrapper around a listening socket.  Modern class.
//---------------------------------------------------------------------------

#include "SCKaddress.h"
#include "SCKsystem.h"

#include <COL/COLclosure.h>
#include <COL/COLauto.h>

class SCKloop;
class SCKconnection;

class SCKlistener{
public:
   SCKlistener(SCKloop& Loop);
   ~SCKlistener();

   // This listen doesn't support doing name lookups since we'd have to provide a callback in case bind failed.
   // BindToIp is an optional argument to bind to a port.
   bool listen(int Port, const COLstring& BindToIp="");
   bool listen(const SCKaddress& Address);
   // returns the listening port. Useful when listen on port 0.
   int port() { return m_Address.port(); }

   void setOnAccept(COLclosure0* pOnAccept);
   // The pRemoteAddress can be null if you aren't interested in the address of the socket connecting
   // to you.  You may need to specify the SCK family
   int accept(SCKaddress* pRemoteAddress=NULL);
   int lastError() const { return m_LastError; }
   int handle()    { return m_Handle;    }
   SCKloop* loop() { return m_pLoop; }
   void close();

   void onAccept();  // Don't call this - called by SCKloop
private:
   COLauto<COLclosure0> m_pOnAccept;
   int m_Handle;
   int m_LastError;
   SCKloop* m_pLoop;
   SCKaddress m_Address;
   void recordLastError();

   SCKlistener(const SCKlistener& Orig); // not allowed
   SCKlistener& operator=(const SCKlistener& Orig); // not allowed
//=================================================================================
// Now implementation specific stuff
#ifdef SCK_IOCP
public:
   void onIocp(const WSAOVERLAPPED& OverLapped,  DWORD CurrentSizeOfData, bool IsLastOpSuccessful);
private:
   int m_NextConnectionHandle;
   COLstring m_AddressBuffer;
   // Only one overlapped structure for Listeners since the only async operation is the Accept operation
   OVERLAPPED m_AcceptOverlapped;
   void startAccept();
#endif // SCK_IOCP
};

#endif // end of defensive include
