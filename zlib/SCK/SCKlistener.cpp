//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: SCKlistener
//
// Author: Eliot Muir
//
// Description:
//
// Implementation
//---------------------------------------------------------------------------

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include "SCKlistener.h"
#include "SCKconnection.h"
#include "SCKloop.h"
#include "SCKutils.h"
#include "SCKerror.h"
#include "SCKaddress.h"

#ifdef SCK_SELECT

SCKlistener::SCKlistener(SCKloop& Loop) : m_pLoop(&Loop), m_Handle(SCK_INVALID_SOCKET), m_LastError(0){
   COL_METHOD(SCKlistener::SCKlistener);
}

SCKlistener::~SCKlistener(){
   COL_METHOD(SCKlistener::~SCKlistener);
   close();
}

void SCKlistener::close(){
   COL_METHOD(SCKlistener::close);
   if (m_Handle != SCK_INVALID_SOCKET){
      m_pLoop->removeAllCallback(m_Handle);
      ::closesocket(m_Handle);
      m_Handle = SCK_INVALID_SOCKET;
   }
}

bool SCKlistener::listen(const SCKaddress& HostIp){
   COL_METHOD(SCKlistener::listen);
   COL_VAR(HostIp);
   m_Address = HostIp;
   if (m_pLoop->maxFd() - m_pLoop->countOfFd() < 1){
      // TODO - if mazFd() - countOfFd < 10 then we should fail with error code EMFILE - TODO - dig into libuv to find out where UV_EMFILE is defined.
   }

   m_Handle = SCKcreateSocket(m_Address.ipFamily());
	fcntl(m_Handle, F_SETFD, FD_CLOEXEC);
   if (m_Handle == SCK_INVALID_SOCKET){
      recordLastError();  // IMPORTANT - recordSocketError before any tracing.
      COL_TRC("Create socket failed.");
      return false;
   }
   COL_VAR(m_Handle);
   SCKsetReuseAddress(m_Handle); 
   if (-1 == ::bind(m_Handle, m_Address.addressPtr(), m_Address.addressSize())){
      recordLastError();  // IMPORTANT - recordSocketError must be done before ANY tracing otherwise the error will be lost.
      COL_ERR("::bind failed for: " << HostIp << " " << SCKsocketError(lastError()) << " : " << lastError());
      close();
      return false;
   }

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
      recordLastError();              // IMPORTANT - recordSocketError must be done before ANY tracing otherwise the error will be lost.
      close();
      return false;
   }

   COL_TRC("Listen succeeded.");
   m_pLoop->registerCallbacks(m_Handle, NULL, COLnewClosure0(this, &SCKlistener::onAccept), NULL);
   m_pLoop->addReadCallback(m_Handle);
   return true;
}

int SCKlistener::accept(SCKaddress* pAddress){
   COL_METHOD(SCKlistener::accept);
   SCKsocketLength AddressLength = pAddress ? pAddress->addressSize() : 0;
   COL_VAR2(m_Handle, AddressLength);
   int PeerHandle = ::accept(m_Handle, pAddress ? pAddress->addressPtr() : NULL, &AddressLength);
   if (PeerHandle == SCK_INVALID_SOCKET){
      recordLastError();
   }
   COL_VAR(PeerHandle);
   return PeerHandle;
}
#endif //SCK_SELECT

void SCKlistener::setOnAccept(COLclosure0* pOnAccept){
   COL_METHOD(SCKlistener::setOnAccept);
   COL_VAR(pOnAccept);
   m_pOnAccept = pOnAccept;
}

// SUPER IMPORTANT to record the last socket error before doing any tracing which
// under windows will clobber the last error code.
void SCKlistener::recordLastError(){
   m_LastError = SCKlastSocketError();
   COL_VAR(m_LastError);
}

void SCKlistener::onAccept(){
   COL_METHOD(SCKlistener::onAccept);
   COLASSERT(m_pOnAccept.get());
   try{
      m_pOnAccept->run();
   } catch (const COLerror& Error){
      // IX-1857 We violently stop - this is a programmer error.
      COLabortWithMessage("Exception thrown from onAccept handler" + Error.description());
   }
}

bool SCKlistener::listen(int Port, const COLstring& Host){
   COL_METHOD(SCKlistener::listen);
   COL_VAR2(Host,Port);
   m_Address.setPort(Port);
   if (!Host.is_null()){
      if (!m_Address.fromString(Host)){
         // We could do name lookups - but it means the caller needs to provide a error call back
         COL_WRN("This interface doesn't support doing host name lookups - need to use an IP address");
         return false;
      }
   }
   return listen(m_Address);
}
