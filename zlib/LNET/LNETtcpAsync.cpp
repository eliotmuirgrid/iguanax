//---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LNETtcpAsync
//
// Description:
//
// The asynchronous socket api for Lua.
//
// Author: John Q
// Date:   Thu Apr 24 11:40:14 EDT 2025
//---------------------------------------------------------------------------
#include "LNETtcpAsync.h"

#include <BLUA/BLUAinstance.h>
#include <BLUA/BLUAsocket.h>
#include <COL/COLlog.h>
#include <LUA/LUAerror.h>
#include <LUA/LUAstartup.h>
#include <LUA/LUAtable.h>
#include <LUA/LUAtest.h>
#include <LUA/LUAutils.h>
#include <LUAL/LUALutils.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

#define LNET_TCP_ASYNC_META            "net_tcp_async_meta"
#define LNET_TCP_ASYNC_ON_ACCEPT       "net_tcp_async_on_accept"
#define LNET_TCP_ASYNC_CONNECTION_MAP  "net_tcp_async_connections"

class LNETsocketAsync {
public:
   void send(COLauto<COLstring> pData) {
      pLoop->post(COLnewClosure0(&BLUAinstanceSendAsync, pInstance, pData, ConnId));
   }
   void close() {
      pLoop->post(COLnewClosure0(&BLUAinstanceCloseAsync, pInstance, ConnId));
   }
   COLstring print() {
      return COL_T("Connection ") + COLintToString(ConnId) + " (" + IpAddress + ")";
   }

   int            ConnId      { -1 };
   int            OnConnect   { 0 };
   int            OnRead      { 0 };
   int            OnWrite     { 0 };
   int            OnClose     { 0 };
   SCKloop*       pLoop       { nullptr };
   BLUAinstance2* pInstance   { nullptr };
   COLstring      IpAddress;
};

static inline LNETsocketAsync* LNETgetSocket(lua_State* L, int i) {
   void* pSock = LUAcheckUdata(L, i, LNET_TCP_ASYNC_META, "Invalid socket");
   return (LNETsocketAsync*) pSock;
}

static int LNETprint(lua_State* L) {
   COL_FUNCTION(LNETprint);
   LNETsocketAsync* pSocket = LNETgetSocket(L, 1);
   lua_pushstring(L, pSocket->print().c_str());
   return 1;
}

static int LNETconcat(lua_State* L) {
   COL_FUNCTION(LNETconcat);
   // COL_VAR(LUAstackAsString(L));
   COLstring Result;
   if (lua_isuserdata(L, 1)) {
      LNETsocketAsync* pSocket = LNETgetSocket(L, 1);
      Result = pSocket->print();
      Result.append(LUAtoString(L, 2));
   } else {
      LNETsocketAsync* pSocket = LNETgetSocket(L, 2);
      Result = LUAtoString(L, 1);
      Result.append(pSocket->print());
   }
   lua_pushstring(L, Result.c_str());
   // COL_VAR(LUAstackAsString(L));
   return 1;
}

static int LNETdestroy(lua_State* L) {
   COL_FUNCTION(LNETdestroy);
   LNETsocketAsync* pSocket = LNETgetSocket(L, 1);
   if (LUALisEnvironmentTest(L)) { return 0; }           // this is a no-op in IDE mode
   pSocket->close();
   if (pSocket->OnConnect) { COL_VAR(pSocket->OnConnect); luaL_unref(L, LUA_REGISTRYINDEX, pSocket->OnConnect); pSocket->OnConnect = 0; }
   if (pSocket->OnRead)    { COL_VAR(pSocket->OnRead);    luaL_unref(L, LUA_REGISTRYINDEX, pSocket->OnRead);    pSocket->OnRead    = 0; }
   if (pSocket->OnWrite)   { COL_VAR(pSocket->OnWrite);   luaL_unref(L, LUA_REGISTRYINDEX, pSocket->OnWrite);   pSocket->OnWrite   = 0; }
   if (pSocket->OnClose)   { COL_VAR(pSocket->OnClose);   luaL_unref(L, LUA_REGISTRYINDEX, pSocket->OnClose);   pSocket->OnClose   = 0; }
   return 0;
}

// pushes the connection object <ConnId> onto the Lua stack
// throws lua error if connection map or <ConnId> doesn't exist
static void LNETpushConnectionObject(lua_State* L, int ConnId) {
   COL_FUNCTION(LNETpushConnectionObject);
   // COL_VAR(LUAstackAsString(L));
   int Top = lua_gettop(L);
   lua_getfield(L, LUA_REGISTRYINDEX, LNET_TCP_ASYNC_CONNECTION_MAP);      // push connection_map
   if (lua_isnil(L, -1)) {
      // async_socket_map should have been created in LNETonAccept
      luaL_error(L, "Failed to retrieve the socket connection map.");
   }
   lua_rawgeti(L, -1, ConnId);                                             // push connection_map[ConnId]
   if (lua_isnil(L, -1)) {
      // socket should have been created in LNETonAccept
      COLstring Error = "Failed to retrieve socket connection " + COLintToString(ConnId);
      luaL_error(L, Error.c_str());
   }
   lua_remove(L, Top + 1);                                                 // pop connection_map
   // COL_VAR(LUAstackAsString(L));
}

static void LNETaddConnection(lua_State* L, int ConnId, const COLstring& IpAddress = "") {
   COL_FUNCTION(LNETaddConnection);
   // COL_VAR(LUAstackAsString(L));
   COL_VAR(ConnId);
   lua_getfield(L, LUA_REGISTRYINDEX, LNET_TCP_ASYNC_CONNECTION_MAP);         // push connection_map
   if (lua_isnil(L, -1)) {
      COL_TRC("Create connection_map");
      lua_pop(L, 1);                                                          // pop nil
      lua_newtable(L);                                                        // push new table as connection_map
      lua_setfield(L, LUA_REGISTRYINDEX, LNET_TCP_ASYNC_CONNECTION_MAP);      // save new table to registry
      lua_getfield(L, LUA_REGISTRYINDEX, LNET_TCP_ASYNC_CONNECTION_MAP);      // push connection_map
   }
   LNETsocketAsync* pSocket = LUApushObject<LNETsocketAsync>(L, LNET_TCP_ASYNC_META);   // push new socket object
   pSocket->ConnId    = ConnId;
   pSocket->IpAddress = IpAddress;
   pSocket->pInstance = BLUAinstanceGet(L);
   pSocket->pLoop     = pSocket->pInstance->m_pLoop;
   lua_rawseti(L, -2, ConnId);                                                // connection_map[ConnId] = pSocket
   lua_pop(L, 1);                                                             // pop connection_map
   // COL_VAR(LUAstackAsString(L));
}

static void LNETremoveConnection(lua_State* L, int ConnId) {
   COL_FUNCTION(LNETremoveConnection);
   // COL_VAR(LUAstackAsString(L));
   COL_VAR(ConnId);
   lua_getfield(L, LUA_REGISTRYINDEX, LNET_TCP_ASYNC_CONNECTION_MAP);   // push connection_map
   if (lua_isnil(L, -1)) {
      COL_WRN("Socket map doesn't exist");
      lua_pop(L, 1);
      return;
   }
   lua_pushnil(L);                                                      // push nil
   lua_rawseti(L, -2, ConnId);                                          // connection_map[ConnId] = nil
   lua_pop(L, 1);                                                       // pop connection_map
   // connection_map[ConnId] = nil marks the LNETsocketAsync object to be garbage collected, but it won't happen immediately.
   // If you want to see __gc (LNETdestory) being called for debugging, uncomment lua_gc to force garbage collection.
   // lua_gc(L, LUA_GCCOLLECT, 0);
   // COL_VAR(LUAstackAsString(L));
}

int LNETonAccept(lua_State* L, int ConnId, const COLstring& ClientIp) {
   COL_FUNCTION(LNETonAccept);
   // COL_VAR(LUAstackAsString(L));
   LNETaddConnection(L, ConnId, ClientIp);
   lua_getfield(L, LUA_REGISTRYINDEX, LNET_TCP_ASYNC_ON_ACCEPT);     // push onAccept
   LNETpushConnectionObject(L, ConnId);                              // push socket
   LUApushString(L, ClientIp);                                       // push clientIp
   int Result = lua_pcall(L, 2, 0, 0);                               // onAccept(socket, clientIp)
   // COL_VAR(LUAstackAsString(L));
   return Result;
}

int LNETonData(lua_State* L, int ConnId, const COLstring& Buffer) {
   COL_FUNCTION(LNETonData);
   // COL_VAR(LUAstackAsString(L));
   LNETpushConnectionObject(L, ConnId);
   LNETsocketAsync* pSocket = (LNETsocketAsync*)lua_touserdata(L, -1);
   lua_pop(L, 1);
   if (0 == pSocket->OnRead) {
      COL_WRN("onRead callback missing");
      LUApushString(L, "The 'onRead' callback function is not set for socket connection " + COLintToString(ConnId));
      return BLUA_ERROR_COMPILE;
   }
   lua_rawgeti(L, LUA_REGISTRYINDEX, pSocket->OnRead);   // push onRead
   LNETpushConnectionObject(L, ConnId);                  // push socket
   LUApushString(L, Buffer);                             // push buffer
   int Result = lua_pcall(L, 2, 0, 0);                   // onRead(socket, Buffer)
   // COL_VAR(LUAstackAsString(L));
   return Result;
}

int LNETonWrite(lua_State* L, int ConnId) {
   COL_FUNCTION(LNETonWrite);
   // COL_VAR(LUAstackAsString(L));
   LNETpushConnectionObject(L, ConnId);
   LNETsocketAsync* pSocket = (LNETsocketAsync*)lua_touserdata(L, -1);
   lua_pop(L, 1);
   if (0 == pSocket->OnWrite) {
      COL_TRC("onWrite callback not set. Skip");
      // COL_VAR(LUAstackAsString(L));
      return 0;
   }
   lua_rawgeti(L, LUA_REGISTRYINDEX, pSocket->OnWrite);  // push onWrite
   LNETpushConnectionObject(L, ConnId);                  // push socket
   int Result = lua_pcall(L, 1, 0, 0);                   // onWrite(socket)
   // COL_VAR(LUAstackAsString(L));
   return Result;
}

int LNETonClose(lua_State* L, int ConnId, const COLstring& Error, const COLstring& Buffer) {
   COL_FUNCTION(LNETonClose);
   // COL_VAR(LUAstackAsString(L));
   LNETpushConnectionObject(L, ConnId);
   LNETsocketAsync* pSocket = (LNETsocketAsync*)lua_touserdata(L, -1);
   lua_pop(L, 1);
   if (0 == pSocket->OnClose) {
      COL_WRN("onClose callback missing");
      LUApushString(L, "The 'onClose' callback function is not set for socket connection " + COLintToString(ConnId));
      return BLUA_ERROR_COMPILE;
   }
   lua_rawgeti(L, LUA_REGISTRYINDEX, pSocket->OnClose);  // push onClose
   LNETpushConnectionObject(L, ConnId);                  // push socket
   LUApushString(L, Buffer);                             // push buffer
   int nArgs = 2;
   if (!Error.is_null()) {
      LUApushString (L, Error);                          // push error
      nArgs++;
   }
   int Result = lua_pcall(L, nArgs, 0, 0);               // onClose(socket, buffer, error)
   LNETremoveConnection(L, ConnId);
   // COL_VAR(LUAstackAsString(L));
   return Result;
}

int LNETonConnect(lua_State* L, int ConnId) {
   COL_FUNCTION(LNETonConnect);
   // COL_VAR(LUAstackAsString(L));
   LNETpushConnectionObject(L, ConnId);
   LNETsocketAsync* pSocket = (LNETsocketAsync*)lua_touserdata(L, -1);
   lua_pop(L, 1);
   if (0 == pSocket->OnConnect) {
      COL_WRN("onConnect callback missing");
      LUApushString(L, "The 'onConnect' callback function is not set for socket connection " + COLintToString(ConnId));
      return BLUA_ERROR_COMPILE;
   }
   lua_rawgeti(L, LUA_REGISTRYINDEX, pSocket->OnConnect);   // push OnConnect
   LNETpushConnectionObject(L, ConnId);                     // push socket
   int Result = lua_pcall(L, 1, 0, 0);                      // OnConnect(socket)
   // COL_VAR(LUAstackAsString(L));
   return Result;
}

static int LNETsend(lua_State* L) {
   COL_FUNCTION(LNETsend);
   LNETsocketAsync* pSocket = LNETgetSocket(L, 1);
   if (!lua_isstring(L, 2)) { luaL_error(L, "Please provide the data to be sent."); }
   if (LUALisEnvironmentTest(L)) { return 0; }           // this is a no-op in IDE mode
   COLauto<COLstring> pData = new COLstring(LUAtoString(L, 2));
   COL_VAR(*pData);
   pSocket->send(pData);
   return 0;
}

static int LNETclose(lua_State* L) {
   COL_FUNCTION(LNETclose);
   LNETsocketAsync* pSocket = LNETgetSocket(L, 1);
   if (LUALisEnvironmentTest(L)) { return 0; }           // this is a no-op in IDE mode
   pSocket->close();
   return 0;
}

static int LNETsetOnRead(lua_State* L) {
   COL_FUNCTION(LNETsetOnRead);
   LNETsocketAsync* pSocket = LNETgetSocket(L, 1);
   if (!lua_isfunction(L, 2)) { luaL_error(L, "A Lua function must be provided to handle the onRead event."); }
   pSocket->OnRead = luaL_ref(L, LUA_REGISTRYINDEX);
   return 0;
}

static int LNETsetOnWrite(lua_State* L) {
   COL_FUNCTION(LNETsetOnWrite);
   LNETsocketAsync* pSocket = LNETgetSocket(L, 1);
   if (!lua_isfunction(L, 2)) { luaL_error(L, "A Lua function must be provided to handle the onWrite event."); }
   pSocket->OnWrite = luaL_ref(L, LUA_REGISTRYINDEX);
   return 0;
}

static int LNETsetOnClose(lua_State* L) {
   COL_FUNCTION(LNETsetOnClose);
   LNETsocketAsync* pSocket = LNETgetSocket(L, 1);
   if (!lua_isfunction(L, 2)) { luaL_error(L, "A Lua function must be provided to handle the onClose event."); }
   pSocket->OnClose = luaL_ref(L, LUA_REGISTRYINDEX);
   return 0;
}

static int LNETlisten(lua_State* L) {
   COL_FUNCTION(LNETlisten);
   // COL_VAR(LUAstackAsString(L));
   int Port = LUAexpectedIntParam(L, 1, "port");
   COL_VAR(Port);
   LUAexpectedFuncParam (L, 1, "onAccept");
   lua_setfield(L, LUA_REGISTRYINDEX, LNET_TCP_ASYNC_ON_ACCEPT);    // save onAccept to registry
   COLstring Cert;
   COLstring Key;
   COLstring CAfile;
   bool VerifyPeer = false;
   if (LUAoptionalTableParam(L, 1, "ssl")){    // ssl is optional, but if it is present, cert key and ca_file are required
      LUApushString(L, "ssl");
      lua_gettable(L, 1);
      Cert = LUAexpectedStringParam(L, 2, "cert");
      Key =  LUAexpectedStringParam(L, 2, "key");
      VerifyPeer = LUAoptionalBoolParam(L, 2, "verify_peer", false);  // optionally, can also try to verify peer with a further optional CA file
      CAfile = LUAoptionalStringParam(L, 2, "ca_file");
      lua_pop(L, 1);
      COL_VAR4(Cert, Key, CAfile, VerifyPeer);
   }

   if (LUALisEnvironmentTest(L)) { return 0; }  // this is a no-op in IDE mode

   COLstring Error;
   BLUAinstance2* pInstance = BLUAinstanceGet(L);
   if (!BLUAinstanceListenAsync(pInstance, Port, Cert, Key, VerifyPeer, CAfile, &Error)) {
      luaL_error(L, Error.c_str());
   }
   // COL_VAR(LUAstackAsString(L));
   return 0;
}

static void LNETcheckConnectCallback(lua_State* L, int i, const COLstring& Key) {
   LUAparamIsTable(L, i);
   if (!LUApushParamByName(L, i, Key)) {
      if (Key == "onSendComplete") { return; }
      COLstring Error = "The 'callbacks' table must provide the '" + Key + "' callback function.";
      luaL_error(L, Error.c_str());
   }
   if (!lua_isfunction(L, -1)) {
      COLstring Error = "'callbacks." + Key + "' must be a Lua function.";
      luaL_error(L, Error.c_str());
   }
}

static int LNETconnect(lua_State* L) {
   COL_FUNCTION(LNETconnect);
   COL_VAR(LUAstackAsString(L));
   COLstring Host = LUAexpectedStringParam(L, 1, "host");
   int Port       = LUAexpectedIntParam   (L, 1, "port");
   bool Secure    = LUAoptionalBoolParam  (L, 1, "secure", false);
   COL_VAR3(Host, Port, Secure);

   COL_TRC("callback");
   LUAexpectedTableParam   (L, 1, "callbacks");  // push callback
   COL_TRC("onConnect");
   LNETcheckConnectCallback(L, 2, "onConnect");        int OnConnect = luaL_ref(L, LUA_REGISTRYINDEX);
   COL_TRC("onSocketClosed");
   LNETcheckConnectCallback(L, 2, "onSocketClosed");   int OnClose   = luaL_ref(L, LUA_REGISTRYINDEX);
   COL_TRC("onDataReceived");
   LNETcheckConnectCallback(L, 2, "onDataReceived");   int OnRead    = luaL_ref(L, LUA_REGISTRYINDEX);
   COL_TRC("onSendComplete");
   LNETcheckConnectCallback(L, 2, "onSendComplete");   int OnWrite   = luaL_ref(L, LUA_REGISTRYINDEX);
   lua_pop(L, 1);                               // pop callback
   if (LUALisEnvironmentTest(L) || LUALisEnvironmentStartup(L)) {
      luaL_unref(L, LUA_REGISTRYINDEX, OnConnect);
      luaL_unref(L, LUA_REGISTRYINDEX, OnRead);
      luaL_unref(L, LUA_REGISTRYINDEX, OnWrite);
      luaL_unref(L, LUA_REGISTRYINDEX, OnClose);
      return 0;
   } // this is a no-op in IDE mode

   int ConnId = 0;
   BLUAinstance2* pInstance = BLUAinstanceGet(L);
   BLUAinstanceConnectAsyncNew(pInstance, Host, Port, Secure, &ConnId);

   LNETaddConnection(L, ConnId);
   LNETpushConnectionObject(L, ConnId);                              // push socket
   LNETsocketAsync* pSocket = (LNETsocketAsync*)lua_touserdata(L, -1);
   pSocket->OnConnect = OnConnect;
   pSocket->OnRead    = OnRead;
   pSocket->OnWrite   = OnWrite;
   pSocket->OnClose   = OnClose;
   // COL_VAR(LUAstackAsString(L));
   return 1;
}

static void LNETcreateTcpAsyncMetaTable(lua_State *L) {
   luaL_newmetatable(L, LNET_TCP_ASYNC_META);  /* metatable for file handles */
   LUAaddMethod(L, "__tostring",&LNETprint);
   LUAaddMethod(L, "__concat",  &LNETconcat);
   LUAaddMethod(L, "__gc",      &LNETdestroy);
   lua_newtable(L);
   LUAaddMethod(L, "send",      &LNETsend);
   LUAaddMethod(L, "close",     &LNETclose);
   LUAaddMethod(L, "setOnDataReceived", &LNETsetOnRead);
   LUAaddMethod(L, "setOnSendComplete", &LNETsetOnWrite);
   LUAaddMethod(L, "setOnSocketClosed", &LNETsetOnClose);
   lua_setfield (L, -2, "__index");
   lua_pop(L, 1);
}

void LNETaddTcpAsyncModule(lua_State* L) {
   COL_FUNCTION(LNETaddTcpAsyncModule);
   lua_newtable(L);
   LUAaddMethod(L, "listen",  &LNETlisten);
   LUAaddMethod(L, "connect", &LNETconnect);
   lua_setfield(L, -2, "tcpAsync");
   // set up meta table for socket Lua user objects
   LNETcreateTcpAsyncMetaTable(L);
}

bool LNETusingTcpAsyncListen(lua_State* L) {
   lua_getfield(L, LUA_REGISTRYINDEX, LNET_TCP_ASYNC_ON_ACCEPT);
   bool Result = !lua_isnil(L, -1);
   lua_pop(L, 1);
   return Result;
}

bool LNETusingTcpAsync(lua_State* L, int ConnId) {
   lua_getfield(L, LUA_REGISTRYINDEX, LNET_TCP_ASYNC_CONNECTION_MAP);   // push connection_map
   if (lua_isnil(L, -1)) {
      lua_pop(L, 1);                                                    // pop connection_map
      return false;
   }
   lua_rawgeti(L, -1, ConnId);                                          // push connection_map[ConnId]
   bool Result = !lua_isnil(L, -1);
   lua_pop(L, 1);                                                       // pop connection_map[ConnId]
   return Result;
}