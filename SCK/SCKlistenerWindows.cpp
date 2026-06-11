//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: SCKlistener
//
// Author: Eliot Muir
//
// Description:
//
// Implementation
//---------------------------------------------------------------------------

#include "SCKsystem.h"
#include "SCKlistener.h"
#include "SCKutils.h"
#include "SCKaddress.h"
#include "SCKloop.h"
#include "SCKerror.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

#ifdef SCK_IOCP

// IOCP populates the address of the connecting socket - what is remotely
// and what address we are connecting on which could differ depending on
// what interface the client connected on - WIFI, Ethernet IP etc.
// IOCP is different in that this information comes in as part of the AcceptEx
// completion.  We need allocate space for it.
const int SCK_ADDRESS_SIZE = sizeof(struct sockaddr_in6) + 16;

SCKlistener::SCKlistener(SCKloop& Loop) : m_pLoop(&Loop), m_LastError(0), m_Handle(SCK_INVALID_SOCKET), m_NextConnectionHandle(SCK_INVALID_SOCKET) {
   COL_METHOD(SCKlistener::SCKlistener);
   memset(&m_AcceptOverlapped, 0, sizeof(WSAOVERLAPPED));
   m_AddressBuffer.setCapacity(SCK_ADDRESS_SIZE*2);
   memset((void*)m_AddressBuffer.c_str(), 0, m_AddressBuffer.capacity());
   COL_VAR(m_AddressBuffer.capacity());
}

SCKlistener::~SCKlistener(){
   COL_METHOD(SCKlistener::~SCKlistener);
   close();
}

static void SCKcleanupHandle(int* pHandle) {
   COL_FUNCTION(SCKcleanupHandle);
   int Handle = *pHandle;
   COL_VAR(Handle);
   BOOL Result = ::CancelIo((HANDLE)Handle);
   if (!Result){
      long Error = ::GetLastError();
      COL_ERR(COLerrorString(Error) << " while cancelling IO on " << Handle);
   }
   COL_VAR(Result);
   ::closesocket(Handle);
   *pHandle = SCK_INVALID_SOCKET;
}

void SCKlistener::close(){
   COL_METHOD(SCKlistener::close);
   COL_VAR(m_Handle);
   if (m_Handle != SCK_INVALID_SOCKET){
      COL_TRC("Remove listener.");
      m_pLoop->removeCallback(m_Handle);
      SCKcleanupHandle(&m_Handle);
      SCKcleanupHandle(&m_NextConnectionHandle);
   }
}

bool SCKlistener::listen(const SCKaddress& HostIp){
   COL_METHOD(SCKlistener::listen);
   COL_TRC("About to log HostIp");
   COL_VAR(HostIp);
   m_Address = HostIp;
   COL_TRC("Did that.");
   m_Handle = SCKcreateSocket(m_Address.ipFamily());
   if (m_Handle == SCK_INVALID_SOCKET){
      recordLastError();  // IMPORTANT - recordSocketError before any tracing.
      COL_ERR(SCKsocketError(lastError()) << " : " << lastError());
      COL_TRC("Create socket failed.");
      return false;
   }
   COL_VAR(m_Handle);

   // TODO - do we need to call this before we do bind and listen?
   loop()->addCallback(m_Handle, COLnewClosure3(this, &SCKlistener::onIocp));
   // TODO - do we need this call or not...
   SCKsetReuseAddress(m_Handle);
   int ReturnCode = 0;
   ReturnCode = ::bind(m_Handle, m_Address.addressPtr(), m_Address.addressSize()); 
   if (-1 == ReturnCode){
      recordLastError();  // IMPORTANT - recordSocketError must be done before ANY tracing otherwise the error will be lost.
      COL_VAR(ReturnCode);
      COL_ERR("::bind failed for: " << HostIp << " " << SCKsocketError(lastError()) << " : " << lastError());
      close();
      return false;
   }
   COL_VAR(ReturnCode);
   COL_TRC("Bind didn't return an error.");
   // get actual port in case we listen on port 0
   if (m_Address.port() == 0) {
      socklen_t Length = m_Address.addressSize();
      if (-1 == ::getsockname(m_Handle, m_Address.addressPtr(), &Length)){
         recordLastError();  // IMPORTANT - recordSocketError must be done before ANY tracing otherwise the error will be lost.
         COL_ERR("::getsockname failed for: " << HostIp << " " << SCKsocketError(lastError()) << " : " << lastError());
         close();
         return false;
      }
   }
   if (-1 == ::listen(m_Handle, 1024)){ // we use 1024 instead of the SOMAXCOMM which is merely 5 for a operating system.
      recordLastError();                // IMPORTANT - recordSocketError must be done before ANY tracing otherwise the error will be lost.
      close();
      return false;
   }
   startAccept();
   return true;
}

void SCKlistener::onIocp(const WSAOVERLAPPED& OverLapped,  DWORD CurrentSizeOfData, bool IsLastOpSuccessful){
   COL_METHOD(SCKlistener::onIocp);
   if(!IsLastOpSuccessful){
      if (m_pLoop->lastError() == ERROR_OPERATION_ABORTED){
         COL_TRC("Yes we stopped listening - no worries.");
         return;
      } 
      COL_VAR(m_NextConnectionHandle);    
      COLstring Error = "TODO - Handle error with accept error = " + COLintToString(m_pLoop->lastError()) + " : " + SCKsocketError(m_pLoop->lastError());
      COL_ERR(Error);
      COLabortWithMessage(Error);
      //SCKcleanupHandle(&m_NextConnectionHandle);
      //startAccept();
      return;
   }
   COL_DUMP("Address Buffer:", m_AddressBuffer.c_str(), m_AddressBuffer.capacity());
   onAccept();
   startAccept();
}

int SCKlistener::accept(SCKaddress* pRemoteAddress){
   COL_METHOD(SCKlistener::accept);
   COL_DUMP("Address Buffer:", m_AddressBuffer.c_str(), m_AddressBuffer.capacity());

   if (pRemoteAddress){
      // It's mind bending how many hoops one has to go through to placate windows...thought I could just read the buffer into SCKaddress
      // but no...you really do need to use this crazy API.
      SOCKADDR* pLocalSockaddr=NULL;
      SOCKADDR* pRemoteSockaddr=NULL;
      int LocalAddressSize=0;
      int RemoteAddressSize=0;
      (*m_pLoop->getAcceptExSockAddrs())((PVOID)m_AddressBuffer.c_str(), 0, SCK_ADDRESS_SIZE, SCK_ADDRESS_SIZE,
         &pLocalSockaddr, &LocalAddressSize, &pRemoteSockaddr, &RemoteAddressSize);
      COL_DUMP("Local Address:", pLocalSockaddr, LocalAddressSize);
      COL_DUMP("Remote Address:", pRemoteSockaddr, RemoteAddressSize);
      pRemoteAddress->copy(pRemoteSockaddr, RemoteAddressSize);
   }
   return m_NextConnectionHandle;
}

void SCKlistener::startAccept(){
   COL_METHOD(SCKlistener::startAccept);
   // TODO - handle IPV6 - yay!
   COL_VAR(m_NextConnectionHandle);
   m_NextConnectionHandle = SCKcreateSocket(AF_INET);
   COL_VAR(m_NextConnectionHandle);
   if (m_NextConnectionHandle == SCK_INVALID_SOCKET){
      COL_ERR("Failed to create connection socket for acceptex");
      return;
   }
   COL_VAR3(SCK_ADDRESS_SIZE, m_Handle, m_NextConnectionHandle);
   COL_TRC("Start accepting asynchronous accept with call to pfAcceptEx.");
   // starts asynchronous accept
   if (!(*m_pLoop->acceptEx())(m_Handle, m_NextConnectionHandle,(PVOID)m_AddressBuffer.c_str(), 0 /* no recv */, SCK_ADDRESS_SIZE, SCK_ADDRESS_SIZE, NULL, &m_AcceptOverlapped)){
      recordLastError();
      if (lastError() != ERROR_IO_PENDING){
         COL_ERR("TODO: Oh - what do I do with this error: " << SCKsocketError(lastError()));
         return;
      } else { COL_TRC("Call to AcceptEx successful - got error ERROR_IO_PENDING"); }
   }
   COL_TRC("Back from call to AcceptEx");
}
#endif // SCK_IOCP
