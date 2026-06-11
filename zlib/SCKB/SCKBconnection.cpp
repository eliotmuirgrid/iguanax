//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKBconnection
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Tue  1 Nov 2022 21:00:25 EDT
//---------------------------------------------------------------------------
#include "SCKBconnection.h"

#include <time.h>

#include <SCKB/SCKBhostResolve.h>

#include <SCK/SCKsystem.h>
#include <SCK/SCKerror.h>
#include <SCK/SCKutils.h>
#include <SCK/SCKaddress.h>

#include <COL/COLerror.h>
#include <COL/COLdateTime.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

SCKBtimeoutSetter::SCKBtimeoutSetter(int Socket, int TimeoutMs, int Direction)
: m_Socket(Socket), m_Timeout(TimeoutMs), m_Direction(Direction) {
   COLASSERT(Direction == SO_RCVTIMEO || Direction == SO_SNDTIMEO);
   if (m_Socket < 0 || m_Timeout <= 0) { return; }
   m_OldTimeout = SCKsetIoTimeout(m_Socket, m_Timeout, m_Direction);
}

SCKBtimeoutSetter::~SCKBtimeoutSetter() {
   if (m_Socket < 0 || m_Timeout <= 0) { return; }
   SCKsetIoTimeout(m_Socket, m_OldTimeout, m_Direction);
}

class SCKBnonBlockingSetter {
public:
   SCKBnonBlockingSetter(int Socket, int TimeoutMs, struct addrinfo* pAddress)
    : m_Socket(Socket), m_Timeout(TimeoutMs), m_pAddress(pAddress) {
      if (m_Timeout <= 0) { return; }
      SCKsetNonblocking(m_Socket, true);
   }
   ~SCKBnonBlockingSetter() {
      freeaddrinfo(m_pAddress);
      if (m_Timeout <= 0) { return; }
      SCKsetNonblocking(m_Socket, false);
   }
private:
   int m_Socket;
   int m_Timeout;
   struct addrinfo* m_pAddress;
};


SCKBconnection::SCKBconnection() {
   COL_METHOD(SCKBconnection::SCKBconnection);
}

SCKBconnection::~SCKBconnection() {
   COL_METHOD(SCKBconnection::~SCKBconnection);
}

static int SCKBcalculateConnectTimeout(int MsTimeoutConfig, int ResolveTime) {
   COL_FUNCTION(SCKBcalculateConnectTimeout);
   ResolveTime *= 1000;    // convert to ms
   if (MsTimeoutConfig <= 0)          { return 0;  }    // no timeout
   if (MsTimeoutConfig < ResolveTime) { return -1; }    // host resolution timed out
   if (MsTimeoutConfig > ResolveTime) { return MsTimeoutConfig - ResolveTime; }   // still have time left
   return 1000;   // spent all of our time on host resolution, but can't return 0, which means no timeout
}

static void SCKBcloseSocket(int& Handle) {
   if (Handle == -1) { return; }
   SCKcloseSocket(Handle);
   Handle = -1;
}

static bool SCKBconnectWithTimeout(int* pHandle, int ConnectTimeout, int* pLastError) {
   COL_FUNCTION(SCKBconnectWithTimeout);
   int Handle = *pHandle;
   int& LastError = *pLastError;
   fd_set WriteSet;
   FD_ZERO(&WriteSet);
   FD_SET(Handle, &WriteSet);
   COL_TRC("Set up connect timeout = " << ConnectTimeout);
   struct timeval Timeout;
   Timeout.tv_sec  =  ConnectTimeout / 1000;
   Timeout.tv_usec = (ConnectTimeout % 1000) * 1000;
   int Result = ::select(Handle+1, NULL, &WriteSet, NULL, &Timeout);
   if (-1 == Result) {
      LastError = errno;
      COL_WRN("select failed with error " << SCKlastSocketError());
      SCKBcloseSocket(*pHandle);
      return false;
   }
   if (0 == Result) {
      COL_TRC("Timeout");
      LastError = SCK_ETIMEDOUT;
      SCKBcloseSocket(*pHandle);
      return false;
   }
   COL_TRC("connect completed before timeout");
   SCKsocketLength Size = sizeof(LastError);
   int ErrorCode = 0;
   ::getsockopt(Handle, SOL_SOCKET, SO_ERROR, (char*)&ErrorCode, &Size);
   if (ErrorCode) {
      LastError = SCKmapErrorCode(ErrorCode);
      COL_VAR(LastError);
      COL_TRC("Failed to connect to host");
      SCKBcloseSocket(*pHandle);
      return false;
   }
   COL_TRC("Connection established");
   LastError = 0;  // clear SCK_EINPROGRESS / SCK_EAGAIN
   return true;
}

bool SCKBconnection::connect(const COLstring& Host, int Port) {
   COL_METHOD(SCKBconnection::connect);
   COL_VAR2(Host, Port);
   // resolve host
   struct addrinfo* Addr;
   time_t ResolveStart = time(NULL);
   if (!SCKBhostResolve(Host, Port, Addr)) {
      COL_TRC("Failed to resolve the host");
      m_LastError = SCK_DNSFAILURE;
      return false;
   }
   time_t ResolveEnd = time(NULL);
   int ResolveTime = ResolveEnd - ResolveStart;
   COL_TRC("Successfully resolved the host in " << ResolveTime << " ms");
   int ConnectTimeout = SCKBcalculateConnectTimeout(m_Timeout, ResolveTime);
   if (-1 == ConnectTimeout) {
      COL_TRC("Spent too much time on host resolution.");
      m_LastError = SCK_ETIMEDOUT;
      return false;
   }
   // connect to host
   COL_VAR(SCKaddressToString(Addr));
   if ((m_Handle = SCKcreateSocket(Addr)) < 0) {
      COL_WRN("Failed create socket to connection to host address");
      m_LastError = m_Handle;
      m_Handle = -1;
      freeaddrinfo(Addr);
      return false;
   }
   SCKBnonBlockingSetter Setter(m_Handle, ConnectTimeout, Addr);
   if (0 == ::connect(m_Handle, Addr->ai_addr, Addr->ai_addrlen)) {
      COL_TRC("Connection established");  // no timeout
      return true;
   }
   m_LastError = SCKlastSocketError();
   COL_VAR2(m_LastError, SCKsocketError(m_LastError));
   if (SCK_EINPROGRESS != m_LastError && SCK_EAGAIN != m_LastError) {
      COL_TRC("Failed to connect to host due to " << SCKsocketError(m_LastError));
      return false;
   }
   return SCKBconnectWithTimeout(&m_Handle, ConnectTimeout, &m_LastError);
}

int SCKBconnection::read(COLstring* pBuffer) {
   COL_METHOD(SCKBconnection::read);
   SCKBtimeoutSetter Setter(m_Handle, m_Timeout, SO_RCVTIMEO);
   int Amount = ::recv(m_Handle, pBuffer->get_buffer(), pBuffer->capacity()-1, 0);
   COL_VAR3(Amount, pBuffer->size(), pBuffer->capacity());
   if (Amount == -1) {
      m_LastError = SCKlastSocketError();
      if (m_LastError == SCK_ETIMEDOUT || m_LastError == SCK_EAGAIN) {
         COL_TRC("Timeout");
         m_LastError = SCK_ETIMEDOUT;
      } else {
         COL_TRC("recv failed with error " << m_LastError);
         SCKBconnection::close();   // make sure we don't call descendent's close()
      }
   } else {
      if (Amount) {
         pBuffer->setSize(Amount);
         COL_DUMP("Buffer = ", pBuffer->c_str(), pBuffer->size());
      } else {
         COL_TRC("connection closed by peer");
         SCKBconnection::close();   // make sure we don't call descendent's close()
      }
   }
   return Amount;
}

int SCKBconnection::write(const COLstring& Buffer) {
   COL_METHOD(SCKBconnection::write);
   COL_DUMP("Buffer = ", Buffer.c_str(), Buffer.size());
   SCKBtimeoutSetter Setter(m_Handle, m_Timeout, SO_SNDTIMEO);
   int Amount = ::send(m_Handle, (const char*)Buffer.get_buffer(), Buffer.size(), 0);
   if (Amount == -1) {
      m_LastError = SCKlastSocketError();
      if (m_LastError == SCK_ETIMEDOUT || m_LastError == SCK_EAGAIN) {
         COL_TRC("Timeout");
         m_LastError = SCK_ETIMEDOUT;
      } else {
         COL_TRC("send failed with error " << m_LastError);
         SCKBconnection::close();   // make sure we don't call descendent's close()
      }
   }
   return Amount;
}

void SCKBconnection::close() {
   COL_METHOD(SCKBconnection::close);
   SCKBcloseSocket(m_Handle);
}
