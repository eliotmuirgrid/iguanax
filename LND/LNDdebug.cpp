//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LNDdebug
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thursday, October 28th, 2010 @ 09:55:16 AM
//---------------------------------------------------------------------------

#include "LNDdebug.h"
#include "LNDutils.h"

#include <NOD/NODaddress.h>

#include <LUA/LUAutils.h>

#include <COL/COLref.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static const char* LND_USE_CALLBACK  = "LND_USE_CALLBACK";
static const char* LND_NODE_CALLBACK = "LND_NODE_CALLBACK";

void LNDuserDataFormatter(lua_State* L, int i, COLostream& Stream){
   COL_FUNCTION(LNDuserDataFormatter);
   COLref<NODplace>* ppPlace = (COLref<NODplace>*)LUAqueryUserData(L, i, LND_TREE_METATABLE);
   if (ppPlace){
      Stream << "Node";
      return;
   }
   LNDplaceAddress* pPlaceWithAddress = (LNDplaceAddress*)LUAqueryUserData(L, i, LND_TREE_METATABLE_WITH_ADDRESS);
   if (pPlaceWithAddress){
      COL_VAR(pPlaceWithAddress->m_Address.parent());
      NODoutputAddress(Stream, pPlaceWithAddress->m_Address.parent()->name(), pPlaceWithAddress->m_Address);
      return;
   }
   Stream << "Unknown UD" << newline;
}

void LNDsetNodDebug(lua_State* L){
   COL_FUNCTION(LNDsetNodDebug);
   lua_pushboolean(L, true);
   lua_setfield(L, LUA_REGISTRYINDEX, LND_USE_CALLBACK);
}

bool LNDnodDebug(lua_State* L){
   COL_FUNCTION(LNDnodDebug);
   lua_getfield(L, LUA_REGISTRYINDEX, LND_USE_CALLBACK);
   bool Result = lua_isnil(L, -1) ? false : true;
   lua_pop(L, 1);
   COL_VAR(Result);
   return Result;
}

// TODO - why do we duplicate this?
void LNDregisterNodeCallback(lua_State* L, TRNannotationState* pNodeCallback){
   COL_FUNCTION(LNDregisterNodeCallback);
   lua_pushlightuserdata(L, (void*)pNodeCallback);
   lua_setfield(L, LUA_REGISTRYINDEX, LND_NODE_CALLBACK);
}

TRNannotationState* LNDgetNodeCallback(lua_State* L){
   COL_FUNCTION(LNDgetNodeCallback);
   lua_getfield(L, LUA_REGISTRYINDEX, LND_NODE_CALLBACK);
   TRNannotationState* pCallback = (TRNannotationState*)lua_touserdata(L, -1);
   lua_pop(L, 1);
   return pCallback;
}

void LNDclearNodeCallback(lua_State* L){
   COL_FUNCTION(LNDclearNodeCallback);
   lua_pushnil(L);
   lua_setfield(L, LUA_REGISTRYINDEX, LND_NODE_CALLBACK);
}
