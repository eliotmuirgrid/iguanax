//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LNETtcp
//
// Description:
//
// Implementation
//
// Author: Eric Mulvaney
// Date:   Wednesday, April 6th, 2011 @ 10:41:50 AM
//
//---------------------------------------------------------------------------
#include "LNETtcp.h"

#include <LUA/LUAheader.h>
#include <LUA/LUAutils.h>
#include <LUA/LUAtimeout.h>
#include <LUA/LUAtable.h>
#include <LUA/LUAerror.h>

#include <SCK/SCKerror.h>

#include <SCKB/SCKBsslConnection.h>
#include <SCKB/SCKBsslContext.h>
#include <COL/COLauto.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


//
// Socket Object
//

const char* LNET_TCP_METATABLE = "tcp_socket";

class LNETtcpSocket {
public:
   void setTimeout(int Milliseconds) { m_Timeout = Milliseconds; }
   void setContext(SCKBsslContext* pContext) { m_pContext = pContext; }

   void connect(lua_State* L, const COLstring& Address, COLuint16 Port, bool Secure = false);
   int  read(lua_State*, COLstring* pBuffer);
   int  write(lua_State*, const COLstring& Buffer);
   void close();
private:
   int m_Timeout { 5000 };  // In milliseconds.
   COLauto<SCKBsslContext> m_pContext;
   COLauto<SCKBconnection> m_pSocket;
};

void LNETtcpSocket::connect(lua_State* L, const COLstring& Host, COLuint16 Port, bool Secure) {
   COL_METHOD(LNETtcpSocket::connect);
   COL_VAR3(Host, Port, Secure);
   if (Secure) {
      COLPRECONDITION(m_pContext.get());
      COL_VAR(m_pContext.get());
      m_pSocket = new SCKBsslConnection(m_pContext.get());
   } else {
      m_pSocket = new SCKBconnection();
   }
   int Left = m_Timeout;
   while (Left > 0) {
      int Wait = COL_MIN(Left, 1000);
      Left -= Wait;
      m_pSocket->setTimeout(Wait);
      if (m_pSocket->connect(Host, Port)) {
         COL_TRC("success");
         return;
      }
      int Error = m_pSocket->lastError();
      if (Error == SCK_ETIMEDOUT) {
         COL_TRC("timeout");
         LUAcheckTimeout(L);
         continue;
      }
      COL_TRC("fail");
      COLstring ErrMsg = SCKsocketError(Error);
      if (Error == SCK_ESSLVERIFY) {
         ErrMsg = "Certificate verification failed: " + m_pContext->lastError();
      }
      throw COLerror(ErrMsg);
   }
   throw COLerror(SCKsocketError(SCK_ETIMEDOUT));
}

int LNETtcpSocket::read(lua_State* L, COLstring* pBuffer) {
   COL_METHOD(LNETtcpSocket::read);
   COL_TRC("Size of read buffer " << pBuffer->capacity());
   int Left = m_Timeout;
   while (Left > 0) {
      int Wait = COL_MIN(Left, 1000);
      Left -= Wait;
      m_pSocket->setTimeout(Wait);
      int Amount = m_pSocket->read(pBuffer);
      if (Amount >= 0) {
         COL_TRC("read " << Amount << " bytes.");
         return Amount;
      }
      int Error = m_pSocket->lastError();
      if (Error == SCK_ETIMEDOUT) {
         COL_TRC("timeout");
         LUAcheckTimeout(L);
         continue;
      }
      COL_TRC("fail");
      throw COLerror(SCKsocketError(Error));
   }
   throw COLerror(SCKsocketError(SCK_ETIMEDOUT));
}

int LNETtcpSocket::write(lua_State* L, const COLstring& Buffer) {
   COL_METHOD(LNETtcpSocket::write);
   COL_VAR(Buffer);
   int Left = m_Timeout;
   while (Left > 0) {
      int Wait = COL_MIN(Left, 1000);
      Left -= Wait;
      m_pSocket->setTimeout(Wait);
      int Amount = m_pSocket->write(Buffer);
      if (Amount >= 0) {
         COL_TRC("wrote " << Amount << " bytes.");
         return Amount;
      }
      int Error = m_pSocket->lastError();
      if (Error == SCK_ETIMEDOUT) {
         COL_TRC("timeout");
         LUAcheckTimeout(L);
         continue;
      }
      COL_TRC("fail");
      throw COLerror(SCKsocketError(Error));
   }
   throw COLerror(SCKsocketError(SCK_ETIMEDOUT));
}

void LNETtcpSocket::close(){
   COL_METHOD(LNETtcpSocket::close);
   m_pSocket->close();
}


static LNETtcpSocket* LNETpushSocket(lua_State* L) {
   COL_FUNCTION(LNETpushSocket);
   return LUApushObject<LNETtcpSocket>(L, LNET_TCP_METATABLE);
}

// net.tcp.connect
static int LNETtcpConnect(lua_State* L) {
   COL_FUNCTION(LNETtcpConnect);
   COLstring Host    = LUAexpectedStringParam(L, 1, "host");
   int       Port    = LUAexpectedIntParam   (L, 1, "port");
   int       Timeout = LUAoptionalIntParam   (L, 1, "timeout", 5);
   bool      Secure  = LUAoptionalBoolParam  (L, 1, "secure");
   if (Port < 0 || Port > 65535) {
      LUA_ERROR_STREAM("port out of range: " << Port);
   } else if(Timeout < 0) {
      LUA_ERROR_STREAM("timeout must not be negative: " << Timeout);
   }
   COL_VAR4(Host, Port, Timeout, Secure);
   SCKBsslContext* pContext = new SCKBsslContext();
   if (LUAoptionalTableParam(L, 1, "ssl")) {
      LUApushString(L, "ssl"); lua_gettable(L, 1);
      pContext->setCaLocations(LUAoptionalStringParam(L, 2, "ca_file"));
      pContext->setVerifyPeer (LUAoptionalBoolParam  (L, 2, "verify_peer", true));
      pContext->setVerifyHost (LUAoptionalBoolParam  (L, 2, "verify_host", true));
      // TODO: other ssl options
      lua_pop(L, 1);
   }
   LNETtcpSocket* pSocket = LNETpushSocket(L);
   pSocket->setContext(pContext);
   pSocket->setTimeout(Timeout * 1000);
   try {
      pSocket->connect(L, Host, Port, Secure);
   } catch(const COLerror& Error) {
      LUA_ERROR_STREAM(Error.description());
   }
   return 1;
}

static inline LNETtcpSocket* LNETgetSocket(lua_State* L, int i) {
   void* pSock = LUAcheckUdata(L, i, LNET_TCP_METATABLE, "Invalid TCP socket");
   return (LNETtcpSocket*) pSock;
}

// Convert a Lua, 1-based range, [*a,*b], to a C range, [*a,*b).
// Output guarantee: 0 <= *a <= *b <= len
static void LNETfixRange(int *a, int *b, int len) {
   int i=*a, j=*b;
   if (i<0) { i+=len; if(i<0) i=0; } else if(i && --i>len) i=len;
   if (j<0) { j+=len+1;            } else if(       j>len) j=len;
   if (j<i) j=i;
   *a=i; *b=j;
}

// net.tcp.send
static int LNETtcpSend(lua_State* L) {
   COL_FUNCTION(tcpSend);
   try {
      size_t Len;
      LNETtcpSocket* pSocket = LNETgetSocket(L,1);
      const char* s = luaL_checklstring(L, 2, &Len);
      int start = luaL_optint(L, 3,   1);
      int end   = luaL_optint(L, 4, (int)Len);
      // Explicit cast to suppress warnings. We've already set end relative
      // to Len, so any loss of precision is already present.
      LNETfixRange(&start, &end, (int)Len);
      int SizeWrote = pSocket->write(L, COLstring(s+start, end-start));
      lua_pushinteger(L, SizeWrote);
   } catch (const COLerror& Error) {
      LUAraiseLuaError(L, Error.description());
   }
   return 1;
}

// net.tcp.receive
static int LNETtcpReceive(lua_State* L) {
   COL_FUNCTION(tcpRecv);
   try {
      LNETtcpSocket* pSocket = LNETgetSocket(L,1);
      COLstring Buffer;
      Buffer.setCapacity(4*1024);
      int Read = pSocket->read(L, &Buffer);
      COL_TRC("Read " << Read << " bytes.");
      if (Read > 0) lua_pushlstring(L, Buffer.c_str(), Read);
      else          lua_pushnil(L);
   } catch (const COLerror& Error) {
      LUAraiseLuaError(L, Error.description());
   }
   return 1;
}

// net.tcp.close
static int LNETtcpClose(lua_State* L) {
   COL_FUNCTION(tcpClose);
   try {
      LNETtcpSocket* pSocket = LNETgetSocket(L,1);
      pSocket->close();
   } catch (const COLerror& Error) {
      LUAraiseLuaError(L, Error.description());
   }
   return 0;
}

static int LNETfreeSocket(lua_State* L) {
   COL_FUNCTION(freeSocket);
   LNETtcpSocket* pSocket = LNETgetSocket(L,1);
   pSocket->close();    // close connection if user stops the component
   pSocket->~LNETtcpSocket();
   return 0;
}

void LNETaddTcpModule(lua_State* L) {
   int Top = lua_gettop(L);
   luaL_newmetatable(L, LNET_TCP_METATABLE);  // Replace if exists.
   lua_newtable(L);
   // Add the tcp module fields (also for the metatable __index).
   LUAaddMethod(L, "connect", LNETtcpConnect);
   LUAaddMethod(L, "send",    LNETtcpSend);
   LUAaddMethod(L, "recv",    LNETtcpReceive);
   LUAaddMethod(L, "receive", LNETtcpReceive);
   LUAaddMethod(L, "close",   LNETtcpClose);
   lua_pushvalue(L, -1);  // Keep a copy for the metatable.
   lua_setfield(L, Top, "tcp");
   // Add fields to the metatable.
   lua_setfield(L, -2, "__index");
   LUAaddMethod(L, "__gc", LNETfreeSocket);
   lua_settop(L, Top);
}
