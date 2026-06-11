//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: Socket API for lua
//
// Implementation
//
// Author: Vismay Shah
// Date:   Tues, June 7th, 2021
//---------------------------------------------------------------------------
#include "LSCKsocketApi.h"

#include <BLUA/BLUAinstance.h>
#include <BLUA/BLUAsocket.h>
#include <COL/COLlog.h>
#include <LUA/LUAutils.h>
#include <LUAL/LUALutils.h>
#include <LUA/LUAstartup.h>
#include <LUA/LUAtable.h>
#include <LUA/LUAutils.h>
#include <LUA/LUAtest.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;


static int LSCKlistenA(lua_State* L) {
   COL_FUNCTION(LSCKlistenA);
   lua_getglobal(L, "socket");
   lua_pushstring(L, "onAccept");
   lua_gettable(L, -2);
   if (!lua_isfunction(L, -1)) {
      luaL_error(L, "socket.onAccept callback function must be set.");
   }
   lua_pop(L, 2);
   int Port = LUAexpectedIntParam(L, 1, "port");
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
   }
   COL_VAR3(Port, Cert, Key);
   if (LUALisEnvironmentTest(L) || LUALisEnvironmentStartup(L)) { return 0; } // this is a no-op in IDE mode

   COLstring Error;
   BLUAinstance2* pInstance = BLUAinstanceGet(L);
   if (!BLUAinstanceListenAsync(pInstance, Port, Cert, Key, VerifyPeer, CAfile, &Error)) {
      luaL_error(L, Error.c_str());
   }
   // TODO: we need an api to stop/restart the server
   return 0;
}

static int LSCKconnectA(lua_State* L) {
   COL_FUNCTION(LSCKconnectA);
   lua_getglobal(L, "socket");
   lua_pushstring(L, "onConnect");
   lua_gettable(L, -2);
   if (!lua_isfunction(L, -1)) {
      luaL_error(L, "socket.onConnect callback function must be set.");
      return 0;
   }
   lua_pop(L, 2);

   COLstring Host = LUAexpectedStringParam(L, 1, "host");
   int Port       = LUAexpectedIntParam   (L, 1, "port");
   bool Secure    = LUAexpectedBoolParam  (L, 1, "secure");
   COL_VAR3(Host, Port, Secure);
   if (LUALisEnvironmentTest(L) || LUALisEnvironmentStartup(L)) { return 0; } // this is a no-op in IDE mode
   BLUAinstance2* pInstance = BLUAinstanceGet(L);
   pInstance->m_pLoop->post(COLnewClosure0(&BLUAinstanceConnectAsync, pInstance, Host, Port, Secure));
   return 0;
}

static int LSCKsendA(lua_State* L){
   COL_FUNCTION(LSCKsendA);
   COLauto<COLstring> pData = new COLstring(LUAexpectedStringParam(L, 1, "data"));
   int ConnId = LUAexpectedIntParam(L, 1, "connection");
   if (LUALisEnvironmentTest(L) || LUALisEnvironmentStartup(L)) { return 0; } // this is a no-op in IDE mode
   // ask BLUAinstance2 to send data
   BLUAinstance2* pInstance = BLUAinstanceGet(L);
   pInstance->m_pLoop->post(COLnewClosure0(&BLUAinstanceSendAsync, pInstance, pData, ConnId));
   return 0;
}

static int LSCKcloseA(lua_State* L){
   COL_FUNCTION(LSCKcloseA);
   int ConnId = LUAexpectedIntParam(L, 1, "connection");
   if (LUALisEnvironmentTest(L) || LUALisEnvironmentStartup(L)) { return 0; } // this is a no-op in IDE mode
   // ask BLUAinstance2 to close connection
   BLUAinstance2* pInstance = BLUAinstanceGet(L);
   pInstance->m_pLoop->post(COLnewClosure0(&BLUAinstanceCloseAsync, pInstance, ConnId));
   return 0;
}

void LSCKaddSocketNamespace(lua_State* L) {
   COL_FUNCTION(LSCKaddSocketNamespace);
   lua_newtable(L);
   LUAaddMethod(L, "listen_a",  &LSCKlistenA);
   LUAaddMethod(L, "connect_a", &LSCKconnectA);
   LUAaddMethod(L, "send_a",    &LSCKsendA);
   LUAaddMethod(L, "close_a",   &LSCKcloseA);
   lua_setglobal(L, "socket");
}
