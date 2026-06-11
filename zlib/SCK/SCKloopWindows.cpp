//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: SCKloopIocp
//
// Author: Eliot Muir
//
// Description:
//
// Implementation - IOCP implementation of socket loop
//---------------------------------------------------------------------------

#include "SCKloop.h"
#include "SCKutils.h"
#include "SCKerror.h"
#include "SCKnameResolver.h"

#include <COL/COLclosure.h> 
#include <COL/COLlist.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


#ifdef SCK_IOCP

SCKloop::SCKloop(COLthreadPool* pPool) : m_pAcceptExFunc(0), m_pConnectExFunc(0), m_EndLoop(false), m_LastError(0), m_pThreadPool(pPool), m_NameResolver(pPool, this), m_StartThread(0) {
   COL_METHOD(SCKloop::SCKloop);
   init();
}

SCKloop::~SCKloop(){
   COL_METHOD(SCKloop::~SCKloop);
   COL_VAR(m_CallbackMap.size());
   if (m_CallbackMap.size() > 0){
      COL_ERR("Warning it looks like some sockets didn't get destroyed: m_CallbackMap.size() = " << m_CallbackMap.size());
      for (auto i=m_CallbackMap.cbegin(); i != m_CallbackMap.cend(); i++){
         COL_ERR("Handle = " << i->first << " not cleaned up.");
      }
   }
   COLASSERT(m_CallbackMap.size() == 0);
}

void SCKloop::init(){
   COL_METHOD(SCKloop::init);
   loadApi();
   createIoPort();
}

// Ah the Majik of windows.  Any other OS would simply have a header file that you would include and link
// to but Windows has this magical API to look up the IOCP functions at runtime by finding a option on a socket.
// I guess in theory that means different socket subsystems can give you different implementations for these extension functions 
void SCKloop::loadApi(){
   COL_FUNCTION(SCKloop::loadApi);
   GUID IdAcceptEx = WSAID_ACCEPTEX;  // defined in msock.h
   GUID IdConnectEx = WSAID_CONNECTEX;
   GUID IdDisconnectEx = WSAID_DISCONNECTEX;
   GUID IdAcceptSockAddrEx = WSAID_GETACCEPTEXSOCKADDRS;

   DWORD Size=0;
   int DummySocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   COL_TRC("Calling WSAIoctl with a dummy socket " << DummySocket << " to load pointers to AcceptEx and ConnectEx at runtime");
   WSAIoctl(DummySocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &IdAcceptEx,  sizeof(IdAcceptEx),  &m_pAcceptExFunc,  sizeof(m_pAcceptExFunc),  &Size, NULL, NULL);
   COL_VAR2(m_pAcceptExFunc, Size);
   WSAIoctl(DummySocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &IdConnectEx, sizeof(IdConnectEx), &m_pConnectExFunc, sizeof(m_pConnectExFunc), &Size, NULL, NULL);
   COL_VAR2(m_pConnectExFunc, Size);
   WSAIoctl(DummySocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &IdDisconnectEx, sizeof(IdDisconnectEx), &m_pDisconnectExFunc, sizeof(m_pDisconnectExFunc), &Size, NULL, NULL);
   COL_VAR2(m_pDisconnectExFunc, Size);
   WSAIoctl(DummySocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &IdAcceptSockAddrEx, sizeof(IdAcceptSockAddrEx), &m_pGetAcceptExSockAddrs, sizeof(m_pGetAcceptExSockAddrs), &Size, NULL, NULL);
   COL_VAR2(m_pGetAcceptExSockAddrs, Size);
   closesocket(DummySocket);
   COL_TRC("After that call.");
   COLASSERT(m_pAcceptExFunc != NULL);
   COLASSERT(m_pConnectExFunc != NULL)
   COLASSERT(m_pDisconnectExFunc != NULL);
   COLASSERT(m_pGetAcceptExSockAddrs != NULL);
}

void SCKloop::createIoPort(){
   COL_METHOD(SCKloop::createIoPort);
   // The first argument to the completion port is an invalid file handle indicating we are not associating this completion port with a given
   // 'file handle' but will associate it later.  Windows is kind of pseudo unixy at this this point where a file handle could be many things...
   // The second argument is NULL meaning we are not associating with an existing completion port.
   // The third argument is NULL - meaning there is no key pointing to some unique bit of data we want to get back when this IO port has a completion thingie for us.
   m_CompletionPort = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
   if (!m_CompletionPort){
      int Error = GetLastError();  // Really SCKlastSocketError etc. are just standard _WIN32 errors.  
      throw COLerror(SCKsocketError(Error), Error);
   }
   COL_VAR(m_CompletionPort);
}

// TODO - three things
// Make the completion key be an incremented value which we manage in this class
// try and cancel requests
// don't launch requests if a socket is marked as deleted.
void SCKloop::start(){
   COL_METHOD(SCKloop::start);
   WSAOVERLAPPED* pCurrentOverLapped(NULL);
   DWORD          CurrentSizeOfData(0);
   bool           IsLastOpSuccessful(false);
   ULONG_PTR pCompletionKey = NULL;
   m_EndLoop = false;
   do{
      IsLastOpSuccessful = GetQueuedCompletionStatus(m_CompletionPort, &CurrentSizeOfData, &pCompletionKey, &pCurrentOverLapped, m_Schedule.timeTillNextEvent(1000));
      if (!IsLastOpSuccessful){
         recordLastError();
         if (lastError() != WAIT_TIMEOUT){
            dispatchToReceiver(pCompletionKey, *pCurrentOverLapped, CurrentSizeOfData, IsLastOpSuccessful);
         }
         m_Schedule.fire();
      } else {
         if (pCurrentOverLapped == NULL){
            COL_TRC("We have a closure signaled from another thread.");
            COLclosure0* pClosure = (COLclosure0*)pCompletionKey;
            try{
               pClosure->run();
            } catch (const COLerror& Error){
               // Eliot - IX-1857 Important that we violently stop when this occurs.
               COLabortWithMessage("Exception thrown from ::post" + Error.description());
            }
            delete pClosure;
         } else {
            dispatchToReceiver(pCompletionKey, *pCurrentOverLapped, CurrentSizeOfData, IsLastOpSuccessful);
         }
      }
      COL_VAR(m_EndLoop);
   }while(!m_EndLoop);
}

void SCKloop::dispatchToReceiver(ULONG_PTR pCompletionKey, const WSAOVERLAPPED& IsCurrentOverLapped, DWORD CurrentSizeOfData, bool IsLastOpSuccessful){
   COL_METHOD(SCKloop::dispatchToReceiver);
   COL_VAR(IsLastOpSuccessful);
   COL_TRC("Completion key = " << hex << "0x" << pCompletionKey);
   COL_VAR(CurrentSizeOfData);
   int SocketHandleId = (int)pCompletionKey;
   COL_VAR(SocketHandleId);

   if (m_CallbackMap.find(SocketHandleId)){  // TODO make this more efficient?
      COLclosure3<const WSAOVERLAPPED&,DWORD, bool>* pReceiverCallback = m_CallbackMap[SocketHandleId].get();
      COLASSERT(pReceiverCallback);
      pReceiverCallback->run(IsCurrentOverLapped,CurrentSizeOfData, IsLastOpSuccessful);
   } else {
      COL_TRC("That socket id has gone...");
   }
}

// SUPER IMPORTANT to record the error before we do tracing which can clear it.
void SCKloop::recordLastError(){
   m_LastError = SCKmapErrorCode(SCKlastSocketError());
   COL_VAR(m_LastError);
}

void SCKloop::post(COLclosure0* pClosure){
   COL_METHOD(SCKloop::post);
   if (0 == ::PostQueuedCompletionStatus(m_CompletionPort, 0, (ULONG_PTR)pClosure, NULL)){
      recordLastError();
      COL_ERR(SCKsocketError(m_LastError) << ':' << m_LastError);
      delete pClosure;
   }
}

#endif // SCK_IOCP
