#ifndef __SCK_CONNECTION_H__
#define __SCK_CONNECTION_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: SCKconnection
//
// Author: Eliot Muir
//
// Description:
//
// Describes a connection socket - both client and server side.
//---------------------------------------------------------------------------

#include "SCKsystem.h"

#include <COL/COLauto.h>
#include <COL/COLclosure.h>

class SCKloop;
class SCKaddress;

class SCKconnection{
public:
   // If setting up a server side connection use SCKlistener::accept to get the SocketHandle.
   // If setting up a client side connection leave SocketHandle as its default. One will be
   // created when calling connect.
   SCKconnection(SCKloop& Loop, int SocketHandle=-1, int BufferInCapacity=10000);
   virtual ~SCKconnection();

   virtual COLstring& bufferIn() { return m_BufferIn; } 
   COLstring& bufferOut(){ 
   #ifdef SCK_IOCP
      return m_UseOut1 ? m_BufferOut1 : m_BufferOut2;
   #else
      return m_BufferOut;
   #endif
   };

   
   void connect(const COLstring& Address, int Port);
   void connect(const SCKaddress& Address);

   void rawWrite(const char* pBuffer, int SizeOfBuffer);
   
   // These functions can be override in a SSL/TLS implementation of the connection
   virtual bool sslEnabled() const { return false; }
   virtual void write(const char* pBuffer, int SizeOfBuffer);
   virtual void onData();

   void writeString(const COLstring& Buffer) { return write(Buffer.c_str(), Buffer.size()); } 
   int lastError() const; // onError() callback is removed. Check lastError if care about errors.

   void close();

   // TODO: On Mac, sends a RST packet no matter what, but doesn't do anything different from close() on windows.
   // However, regular close() actually does flush the buffer and acts like an abortive close on all platforms.
   // Revisit this and the close implementation in the future.
   void hardClose();
   
   // WARNING - cleanup deletes this connection.
   void cleanUp();
   void sendData();

   bool m_ClosePending;
   bool m_HardClose;
   int m_TotalRead;
   int m_TotalWritten;

   COLuint64 m_Id;

   // This ensure that only the top level call will delete this object. I.e. if we have the following stack:
   // read()->readcallback()->write()->writecallback()
   // if writecallback() calls close(), then write() will delete the object. However, the stack will then pop to
   // read(), and the object tries to delete itself again.
   // 
   // Solution - Wrap all callbacks with ++m_CallbackStack; callback(); --m_CallbackStack; and only delete when
   // m_CallbackStack == 0;
   int m_CallbackStack;

   COLauto<COLclosure1<COLuint64> > m_pOnConnect;
   COLauto<COLclosure2<COLuint64, COLstring*> > m_pOnData;
   COLauto<COLclosure1<COLuint64> > m_pOnWritable;
   COLauto<COLclosure1<COLuint64> > m_pOnClose;

   bool checkPendingClose();
   void recordLastError();
   SCKloop* m_pLoop;
   int      m_Handle;
   int      m_LastError;
   bool     m_ConnectInProgress;
   COLstring m_BufferIn;

private:
   SCKconnection(const SCKconnection& Orig); // not allowed
   SCKconnection& operator=(const SCKconnection& Orig); // not allowed
//====================================================================================
// Stop reading here if you want to know the public API of socket - everything below is implementation specific.

#ifdef SCK_SELECT
public:
   // Called by SCKselect - don't call.  This is really implementation
   void onWritableEvent();
   void onReadable();
//#ifdef _WIN32
   void onErrorCallback();
//#endif

   COLstring m_BufferOut;
   bool       m_Writable;
private:
   int read (const void* pBuffer, int SizeOfBuffer);
  

#endif // SCK_SELECT

#ifdef SCK_IOCP
public:
   void onIocp(const WSAOVERLAPPED& pOverlapped, DWORD CurrentSizeOfData, bool IsLastOpSuccessful);
   
   // We have two overlapped structures - since we have two kinds of IOCP events, reads and writes.
   OVERLAPPED m_WriteOverlapped;
   OVERLAPPED m_ReadOverlapped;

   WSABUF m_WriteBuffer;
   WSABUF m_ReadBuffer;

   bool m_ReadInProgress;

   // Notice this interesting double buffer setup?  The problem with IOCP is that once you give the buffer to the IOCP call you're not allowed to mess
   // with it on the main thread since IOCP is done with threads.  So I have to switch between two buffers so that data can be buffered while we wait for
   // the other one to return.
   COLstring m_BufferOut1;
   COLstring m_BufferOut2;
   bool m_UseOut1;
   bool m_Writable;   // An unconnected socket is not writable, also this is false if we have a write request going on.

   void onIocpRead(DWORD CurrentSizeOfData, bool IsLastOpSuccessful);
   void onIocpWrite(DWORD CurrentSizeOfData, bool IsLastOpSuccessful);

   void startRead();
   void startWrite();

#endif // SCK_IOCP
};

void SCKshutdown(SCKconnection* pConnection);

#endif // end of defensive include
