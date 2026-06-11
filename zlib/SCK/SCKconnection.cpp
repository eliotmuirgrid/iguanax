//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: SCKconnection
//
// Author: Eliot Muir
//
// Description:
//
// Implementation.
//---------------------------------------------------------------------------

#include <COL/COLclosure.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include "SCKaddress.h"
#include "SCKerror.h"
#include "SCKutils.h"
#include "SCKnameResolver.h"

#include "SCKloop.h"

#include "SCKconnection.h"

#include <SCK/SCKtruncate.h>
#include <SCK/SCKonConnect.h>
#include <SCK/SCKonData.h>
#include <SCK/SCKonClose.h>
#include <SCK/SCKonWritable.h>
#include <SCK/SCKwrite.h>
#include <SCK/SCKconnectionRegister.h>
#include <SCK/SCKconnectionNameResolved.h>

#ifdef SCK_SELECT // IMPORTANT: The code in this ifdef is NOT shared with windows implementation.

// TODO Perhaps consider a similar solution to SCKSsslConnection #26502

SCKconnection::SCKconnection(SCKloop& Loop, int SocketHandle, int BufferInCapacity) 
: m_pLoop(&Loop), m_Handle(SocketHandle), m_LastError(0), m_Writable(m_Handle != SCK_INVALID_SOCKET), m_ConnectInProgress(false),
  m_TotalRead(0), m_TotalWritten(0), m_ClosePending(false), m_HardClose(false), m_CallbackStack(0){
   COL_METHOD(SCKconnection::SCKconnection);
   m_BufferIn.setCapacity(BufferInCapacity);
   if (SocketHandle != SCK_INVALID_SOCKET){
      COL_TRC("Called with valid socket:" << m_Handle);
      int LastError;
      SCKdisableNagleAlgorithm(m_Handle, &LastError);
      SCKsetNonblockingOptions(m_Handle);
      //SCKsetV6DualStack(Address.ipFamily(), m_Handle);  // does this matter?????
      Loop.registerCallbacks(m_Handle, COLnewClosure0(this, &SCKconnection::onWritableEvent),
                                       COLnewClosure0(this, &SCKconnection::onReadable),
                                       COLnewClosure0(this, &SCKconnection::onErrorCallback));
      Loop.addReadCallback(m_Handle);
   }
   SCKconnectionRegister(m_pLoop, this);
}

SCKconnection::~SCKconnection(){
   COL_METHOD(SCKconnection::~SCKconnection);
   COL_VAR(&m_pOnData);
   COL_VAR(m_pOnData.get());
}

void SCKconnection::connect(const SCKaddress& Address){
   COL_METHOD(SCKconnection::connect);
   COL_VAR(Address);
   COLASSERT(m_Handle == SCK_INVALID_SOCKET);
   m_LastError = 0;
   m_Handle = SCKcreateSocket(Address.ipFamily());
   if (m_Handle == SCK_INVALID_SOCKET){
      recordLastError();
      COL_ERR("Failed to create socket.");
      close();
      checkPendingClose();
      return;
   }
   // We're going to assume that errors from these options are non fatal and not useful for the application so no error checking
   int LastError;
   SCKdisableNagleAlgorithm(m_Handle, &LastError);
   SCKsetNonblockingOptions(m_Handle);
   SCKsetV6DualStack(Address.ipFamily(), m_Handle);
   if (::connect(m_Handle, Address.addressPtr(), Address.addressSize()) != 0){ // potential connect error
      recordLastError();  // SUPER IMPORTANT WE CALL THIS BEFORE ANY TRACING.
      switch (m_LastError){
      case SCK_EINPROGRESS:
      case SCK_EAGAIN:
         COL_TRC("SCK_EINPROGRESS and SCK_EWOULDBLOCK doesn't count as an error");
         break;
      default:
         COL_ERR("Connect failed.");
         ::closesocket(m_Handle);
         m_Handle = SCK_INVALID_SOCKET;
         close();
         checkPendingClose();
         return;
      }
   }
   // TODO one day we might optimize this so that COLclosure0s are not created each time (or not)
   m_pLoop->registerCallbacks(m_Handle, COLnewClosure0(this, &SCKconnection::onWritableEvent),
                                        COLnewClosure0(this, &SCKconnection::onReadable),
                                        COLnewClosure0(this, &SCKconnection::onErrorCallback));

   m_pLoop->addReadCallback (m_Handle);
   m_pLoop->addWriteCallback(m_Handle);
#ifdef _WIN32
   m_pLoop->addErrorCallback(m_Handle);
#endif 
   m_ConnectInProgress = true;
   COL_VAR(m_ConnectInProgress);
   COL_TRC("Returned true from connect.");
}

void SCKshutdown(SCKconnection* pConnection){
   COL_FUNCTION(SCKshutdown);
   COL_TRC("Error shutdown sequence.");
   pConnection->close();
   pConnection->checkPendingClose();
}

// TODO refactor...
void SCKconnection::onWritableEvent(){
   COL_METHOD(SCKconnection::onWritableEvent);
   COL_VAR(m_ConnectInProgress);
   COL_VAR(m_BufferOut.size());
   m_Writable = true;
   if (m_ConnectInProgress){
      COL_TRC("We have connected.");
      m_ConnectInProgress = false;
      // KQUEUE BUG : Do a dummy send to see if it truly is connected up
      //    For whatever reason, the onWritableEvent gets triggered before the server actually
      //    calls ::listen, and we'll get a broken pipe error when we try to write
      int DummyBytesSent = ::send(m_Handle, "", 0, 0);
      if (DummyBytesSent == -1) {
         recordLastError();  // Must do this before we do any tracing.
         if (m_LastError != SCK_EAGAIN) {
            if (m_LastError != SCK_EPIPE) {
               close();
               checkPendingClose();
            } else {
               COL_TRC("EPIPE - other side not accepting bytes just yet");
            }
            return;
         }
      }
      if (!SCKonConnect(this)){
         return;  // it can return false if the user code closes the connection.
      } 
   }
   if (m_BufferOut.size() == 0){
      COL_TRC("Nothing to write.");
      m_pLoop->removeWriteCallback(m_Handle);  // Once have have connected or we have written all the data we can we don't want to constantly called back that the socket is writeable
      checkPendingClose();
      return;
   }
   int BytesSent = SCKwrite(this);
   if (BytesSent == -1){
      return;
   }
   if (BytesSent == 0){
      SCKshutdown(this);
      return;
   }
}

void SCKconnection::rawWrite(const char* pBuffer, int SizeOfBuffer){
   COL_METHOD(SCKconnection::write);
   COL_VAR(SizeOfBuffer);
   COL_DUMP("Write:", pBuffer, SizeOfBuffer);
   m_BufferOut.append((const char*)pBuffer, SizeOfBuffer);
   if (!m_Writable){
      return;
   }
   int BytesSent = SCKwrite(this);
   if (BytesSent == -1){
      return;
   }
}

void SCKconnection::onReadable(){
   COL_METHOD(SCKconnection::onReadable);
   COL_VAR2(m_BufferIn.capacity(), m_BufferIn.size());
   // TODO - have to exercise this with large data sets. Buffer can now resize itself so this should be fine now.
   int AmountCanRead = m_BufferIn.capacity() - m_BufferIn.size()-1;
   // Added logic to handle doubling the buffer as needed.
   if (AmountCanRead == 0) {
      COL_TRC("Buffer too small, doubling size...");
      m_BufferIn.setCapacity(m_BufferIn.capacity() * 2);
      COL_VAR2(m_BufferIn.capacity(), m_BufferIn.size());
      AmountCanRead = m_BufferIn.capacity() - m_BufferIn.size()-1;
   }
   int AmountRead = read(m_BufferIn.c_str()+m_BufferIn.size(), AmountCanRead);
   if (AmountRead <= 0){
      SCKshutdown(this);
      return;
   }
   m_BufferIn.setSize(m_BufferIn.size() + AmountRead);
   onData();
   checkPendingClose();
}

int SCKconnection::read(const void* pBuffer, int SizeOfBuffer){
   COL_METHOD(SCKconnection::read);
   if (m_LastError == SCK_ECONNREFUSED){  // we might want to make this _WIN32 only code?
      return -1;
   }
   int BytesRead = ::recv(m_Handle, (char*) pBuffer, SizeOfBuffer, 0);
   if (BytesRead == -1){
      recordLastError();  // SUPER IMPORTANT - record error before tracing.
      COL_VAR(BytesRead);
      return BytesRead;
   }
   m_TotalRead += BytesRead;
   COL_VAR3(SizeOfBuffer, BytesRead, m_TotalRead);
   COL_DUMP("Read:", pBuffer, BytesRead);
   return BytesRead;
}

//#ifdef _WIN32
// Windows is different in how it gives us back a connection refused error.  We try and
// shape it into the same behavior we see on POSIX by changing the error code and invoking
// onReadable which will invoke the runnable that we get with having data to read on the socket
// SCKconnection::read will return -1 when the m_LastError = ECONNREFUSED.  This feels like this
// won't be the end of this.
void SCKconnection::onErrorCallback(){
   COL_METHOD(SCKconnection::onErrorCallback);
   // TODO this is all wrong...it's been made out of date.
   SCKsocketLength ErrorCodeLength = sizeof(m_LastError);
   getsockopt(m_Handle, SOL_SOCKET, SO_ERROR, (char*)&m_LastError, &ErrorCodeLength);
   COL_VAR(m_LastError);
   if (m_LastError == SCK_EADDRNOTAVAIL){
      // indicates a connect error
      m_LastError = SCK_ECONNREFUSED;
      COL_TRC("Change last error to " << m_LastError);
      onReadable();
   } else {
      COL_ERR("Got an error code we didn't anticipate..." << m_LastError);
   }
}
//#endif
#endif // SCK_SELECT

// IMPORTANT: The following code is shared between IOCP implementation and *nix implementation.


// This can be overriden by the SSL/TLS implementation of a connection
void SCKconnection::write(const char* pBuffer, int SizeOfBuffer){
   COL_FUNCTION(SCKconnection::write);
   rawWrite(pBuffer, SizeOfBuffer);
}

void SCKconnection::onData(){
   COL_METHOD(SCKconnection::onData);
   SCKonData(this);
}

// SUPER IMPORTANT to record the error before we do tracing which can clear it.
void SCKconnection::recordLastError(){
   m_LastError = SCKlastSocketError();
   COL_VAR(m_LastError);
}

void SCKconnection::connect(const COLstring& Host, int Port){
   COL_METHOD(SCKconnection::connect);
   COL_VAR2(Host,Port);
   SCKaddress Address(Host, Port);
   Address.setHostName(Host);
   m_pLoop->nameResolver().resolveStart(Address, COLnewClosure1(&SCKconnectionNameResolved, m_pLoop, m_Id));
}


int SCKconnection::lastError() const {
   if(m_LastError == SCK_EINPROGRESS || m_LastError == SCK_EAGAIN) {
      return 0; // SCK_EINPROGRESS and SCK_EWOULDBLOCK doesn't count as an error, so return 0 indicating no error
   }
#ifdef _WIN32  
   if (m_LastError == WSA_IO_PENDING || m_LastError == ERROR_IO_PENDING) return 0;
#endif
   return m_LastError; 
}
