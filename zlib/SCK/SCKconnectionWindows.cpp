//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: SCKconnectionIocp
//
// Author: Eliot Muir
//
// Description:
//
// Implementation - IOCP implementation of connection socket.
//---------------------------------------------------------------------------

#include "SCKsystem.h"
#include "SCKconnection.h"
#include "SCKutils.h"
#include "SCKaddress.h"
#include "SCKloop.h"
#include "SCKerror.h"

#include <SCK/SCKcallStack.h>
#include <SCK/SCKonConnect.h>
#include <SCK/SCKonWritable.h>
#include <SCK/SCKonData.h>
#include <SCK/SCKonClose.h>
#include <SCK/SCKconnectionRegister.h>

#include <COL/COLmath.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#ifdef SCK_IOCP

SCKconnection::SCKconnection(SCKloop& CompletionPort, int SocketHandle, int BufferInCapacity)
: m_Handle(SocketHandle), m_pLoop(&CompletionPort), m_LastError(0), m_UseOut1(true), 
  m_ClosePending(false), m_HardClose(false), m_Writable(SocketHandle != SCK_INVALID_SOCKET), 
  m_ReadInProgress(false), m_ConnectInProgress(false), m_TotalRead(0), m_TotalWritten(0), m_CallbackStack(0){
   COL_METHOD(SCKconnection::SCKconnection);
   COL_VAR3(m_UseOut1, m_Writable, m_ReadInProgress);
   memset(&m_ReadOverlapped, 0, sizeof(WSAOVERLAPPED));
   memset(&m_WriteOverlapped, 0, sizeof(WSAOVERLAPPED));
   COL_VAR3(&m_ReadOverlapped, &m_WriteOverlapped, BufferInCapacity);
   m_BufferIn.setCapacity(BufferInCapacity);
   COL_VAR(m_Handle);
   if (m_Handle != SCK_INVALID_SOCKET){
      COL_TRC("Registering for socket events.");
      m_pLoop->addCallback(m_Handle, COLnewClosure3(this, &SCKconnection::onIocp));
      startRead();
   }
   SCKconnectionRegister(&CompletionPort, this);
}

SCKconnection::~SCKconnection(){
   COL_METHOD(SCKconnection::~SCKconnection);
}

void SCKconnection::connect(const SCKaddress& Address){
   COL_METHOD(SCKconnection::connect);
   COL_VAR(Address);
   COL_VAR2(m_Writable, m_UseOut1);
   m_LastError = 0;
   m_Handle = SCKcreateSocket(Address.ipFamily());
   if (m_Handle == SCK_INVALID_SOCKET){
      recordLastError();
      COL_ERR("Failed to create socket.");
      return;
   }
   COL_VAR(m_Handle);
   SCKaddress LocalAddress;  // If I don't bind the socket before I call ConnectEx then ConnectEx fails with a 'helpful' invalid argument message.
   COL_VAR(LocalAddress);
   LocalAddress.setIpv6(Address.isIpv6());
   if (SOCKET_ERROR == ::bind(m_Handle, LocalAddress.addressPtr(), LocalAddress.addressSize())){
      recordLastError();
      COL_ERR("Failed to bind connection socket." << SCKsocketError(m_LastError));
      cleanUp();
      return;
   } else { COL_TRC("Successfully called bind on socket.");}
   // Note that the last parameter 0 (number of threads) is ignored since we give an existing completion port.
   
   m_pLoop->addCallback(m_Handle, COLnewClosure3(this, &SCKconnection::onIocp)); // TODO Now does the COLclosure really belong as a member of this class or should it be owned by SCKiocp.

   // We are trying writing out data that will be send as part of this call.  TODO - Might later optimize not sending data if buffer size is zero.
   if (!(*m_pLoop->connectEx())(m_Handle, Address.addressPtr(), Address.addressSize(), (PVOID)m_BufferOut1.c_str(), m_BufferOut1.size(),0, &m_WriteOverlapped)){
      recordLastError();
      if (m_LastError != ERROR_IO_PENDING){
         COL_ERR(SCKsocketError(m_LastError));
         cleanUp();
         return;
      }
      COL_TRC("Called ConnectEx successfully - just got ERROR_IO_PENDING");
   } else { COL_TRC("Called ConnectEx successfully!"); }
   m_Writable = false;
   m_UseOut1 = false;
   m_ConnectInProgress = true;
   COL_VAR3(m_Writable, m_UseOut1, m_ConnectInProgress);

   // TODO - might want to have this code kick in when we connect the socket - it fails in this context.
   //if (SOCKET_ERROR == setsockopt( m_Handle, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, NULL, 0 )){  // This will preserve socket options.
   //   recordLastError();
   //   COL_ERR("Updating the socket context didn't work :-( " << SCKsocketError(m_LastError));
   //}
}

void SCKconnection::onIocpWrite(DWORD CurrentSizeOfData, bool IsLastOpSuccessful){
   COL_METHOD(SCKconnection::onIocpWrite);
   COL_VAR(m_Handle);
   COL_VAR2(CurrentSizeOfData, IsLastOpSuccessful);
   COL_TRC("Write operation completed.");
   if(!IsLastOpSuccessful){
      m_LastError = m_pLoop->lastError();
      COL_VAR(m_LastError);
      close();
      checkPendingClose();
      return;
   }
   if (m_ConnectInProgress){
      COL_TRC("Connect completed so fire event.");
      m_ConnectInProgress = false;
      if (!SCKonConnect(this)){
         return;
      }
   }
   // TODO - we're going to have to see IOCP can return with only having written some of the data at which point
   // my clearing the write buffer won't be true.  Likewise what if SizeOfData returned is Zero?
   m_Writable = true;
   int SizeOfData = CurrentSizeOfData;
   m_TotalWritten += SizeOfData;
   COL_VAR4(m_TotalWritten, m_UseOut1, m_BufferOut1.size(), m_BufferOut2.size());
   if (m_UseOut1){  // We just wrote from Buffer2 so we need to clear buffer 2 and check to see if buffer 1 needs to be sent.
      COL_VAR3(m_BufferOut1.size(), m_BufferOut2.size(), SizeOfData);
      // When a socket connection is closed I don't think this assert is always true - see IX-1850
      //COLASSERT(m_BufferOut2.size() == SizeOfData); 
      m_BufferOut2.clear();
      if (m_BufferOut1.size()){
         startWrite();
      } else {
         COL_TRC("No remaining data in buffer 1 to write so call onWritable.");
         SCKonWritable(this);
      }
   } else {  // We just wrote from Buffer1 so we need to clear buffer 1 and check to see if buffer 2 needs to be sent.
      COL_VAR2(m_BufferOut1.size(), SizeOfData);
      COL_DUMP("BufferOut1:", m_BufferOut1.c_str(), m_BufferOut1.size());
      // When a socket connection is closed I don't think this assert is always true - see IX-1850      
      //COLASSERT(m_BufferOut1.size() == SizeOfData);
      m_BufferOut1.clear();
      if (m_BufferOut2.size()){
         startWrite();
      } else {
         COL_TRC("No remaining data in buffer 2 to write so call onWritable.");
         SCKonWritable(this);
      }
   }
   COL_VAR(m_ReadInProgress);
   if (checkPendingClose()){
      return;
   }
   if (!m_ReadInProgress){
      startRead();
   } 
}

void SCKconnection::onIocpRead(DWORD CurrentSizeOfData, bool IsLastOpSuccessful){
   COL_METHOD(SCKconnection::onIocpRead);
   COL_TRC("Read operation completed.");
   int SizeOfData = CurrentSizeOfData;
   COL_VAR(SizeOfData);
   if(!IsLastOpSuccessful){
      m_LastError = m_pLoop->lastError();
      COL_VAR(m_LastError);
      close();
      checkPendingClose();
      return;
   }
   if (SizeOfData == 0){
      COL_TRC("Connection has been closed");
      close();
      checkPendingClose();
      return;
   }
   m_BufferIn.setSize(m_BufferIn.size()+SizeOfData);
   m_ReadInProgress = false;
   m_TotalRead += SizeOfData;
   COL_DUMP("m_BufferIn", m_BufferIn.c_str(), m_BufferIn.size());
   COL_VAR(m_TotalRead);
   onData();
   if (checkPendingClose()){
      return;
   }
   startRead(); 
}

// TODO nicer if we used COLclosure1 and returned the current overlapped in the call.
void SCKconnection::onIocp(const WSAOVERLAPPED& Overlapped, DWORD CurrentSizeOfData, bool IsLastOpSuccessful){
   COL_METHOD(SCKconnection::onIocp);
   SCK_INC_CALLSTACK(this);
   // TODO - we should check whether we get FALSE returned for the success and do error handling.
   COL_VAR3(&Overlapped, &m_ReadOverlapped, &m_WriteOverlapped);
   if (&Overlapped == &m_ReadOverlapped){
      onIocpRead(CurrentSizeOfData, IsLastOpSuccessful);
   } else if (&Overlapped == &m_WriteOverlapped){
      onIocpWrite(CurrentSizeOfData, IsLastOpSuccessful);
   }
   SCK_DEC_CALLSTACK(this);
   checkPendingClose();
}

// TODO - be interesting to see if there are any performance penalties from only having one async write on the go.
void SCKconnection::rawWrite(const char* pData, int Size){
   COL_METHOD(SCKconnection::rawWrite);
   COL_VAR3(m_UseOut1, m_Writable, Size);
   COL_DUMP("Write: ", pData, Size);
   if (m_UseOut1){
      m_BufferOut1.append(pData, Size);
      COL_DUMP("m_BufferOut1: ", m_BufferOut1.c_str(), m_BufferOut1.size());
   } else {
      m_BufferOut2.append(pData, Size);
      COL_DUMP("m_BufferOut2: ", m_BufferOut2.c_str(), m_BufferOut2.size());
   }
   if (m_Writable){
      startWrite();
   } else {
      COL_TRC("Not writable this stage so data is buffered.");
   }
}

void SCKconnection::startWrite(){
   COL_METHOD(SCKconnection::startWrite);
   COL_VAR3(m_Handle, m_Writable, m_UseOut1);
   COLASSERT(m_Writable);
   if (m_UseOut1){
      COL_TRC("Write from buffer out1");
      m_WriteBuffer.buf = (CHAR*)m_BufferOut1.c_str();
      m_WriteBuffer.len = m_BufferOut1.size();
   } else {
      COL_TRC("Write from buffer out2");
      m_WriteBuffer.buf = (CHAR*)m_BufferOut2.c_str();
      m_WriteBuffer.len = m_BufferOut2.size();
   }
   // WSASend supports arrays of buffers - aka gather/scatter stuff - probably not necessary for us.
   if (SOCKET_ERROR == WSASend(m_Handle, &m_WriteBuffer, 1, NULL, 0, &m_WriteOverlapped, NULL)){
      recordLastError();
      if (m_LastError != WSA_IO_PENDING){
         COL_ERR(SCKsocketError(m_LastError));
         close();
         checkPendingClose();
         return;
      }
   }
   m_Writable = false;
   m_UseOut1 = !m_UseOut1;
   COL_VAR2(m_UseOut1, m_Writable);
}

void SCKconnection::startRead(){
   COL_METHOD(SCKconnection::startRead);
   COLASSERT(m_ReadInProgress == false);
   // TODO - we'll need to put in logic to enlarge the buffer when it gets too small.
   if (m_BufferIn.capacity() - m_BufferIn.size() - 1 < 5000){
      m_BufferIn.setCapacity(m_BufferIn.capacity()*2);
   }
   const char* pStartOfInBuffer = m_BufferIn.c_str() + m_BufferIn.size();
   m_ReadBuffer.buf = (CHAR*)pStartOfInBuffer;
   m_ReadBuffer.len = m_BufferIn.capacity() - m_BufferIn.size()-1;
   COL_VAR(m_ReadBuffer.len);
   DWORD Flags = 0;
   // 3rd parameter is 1 which means we are sending just one buffer.
   if (SOCKET_ERROR == WSARecv(m_Handle, &m_ReadBuffer, 1, NULL, &Flags, &m_ReadOverlapped, NULL)){
      recordLastError();
      if (m_LastError != WSA_IO_PENDING){
         COL_ERR(SCKsocketError(m_LastError));
         close();
         checkPendingClose();
         return;
      }
   }
   m_ReadInProgress = true;
   COL_VAR(m_ReadInProgress);
}

#endif // SCK_IOCP

