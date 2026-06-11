// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAmessage
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Tuesday 26 September 2023 - 12:43PM
// ---------------------------------------------------------------------------

#include <LUA/LUAmessage.h>
#include <LUA/LUAutils.h>

#include <COL/COLlog.h>

#define LUA_MESSAGE_INFO_COMPONENT "LUA_MESSAGE_INFO_COMPONENT"
#define LUA_MESSAGE_INFO_DATETIME  "LUA_MESSAGE_INFO_DATETIME"
#define LUA_MESSAGE_INFO_ID        "LUA_MESSAGE_INFO_ID"

COL_LOG_MODULE;

void LUAsetupDefaultMessage(lua_State* L){
   COL_FUNCTION(LUAsetupDefaultMessage);
   LUAsetMessage(L, "", 0, 0);
}

void LUAsetMessage(lua_State* L, const COLstring& ComponentId, COLuint64 DateTime, COLuint64 Index){
   COL_FUNCTION(LUAsetMessage);
   LUApushString(L, ComponentId);
   lua_setfield(L, LUA_REGISTRYINDEX, LUA_MESSAGE_INFO_COMPONENT);
   lua_pushinteger(L, DateTime);
   lua_setfield(L, LUA_REGISTRYINDEX, LUA_MESSAGE_INFO_DATETIME);
   lua_pushinteger(L, Index);
   lua_setfield(L, LUA_REGISTRYINDEX, LUA_MESSAGE_INFO_ID);
}

void LUAgetMessage(lua_State* L, COLstring* pComponentId, COLuint64* pDateTime, COLuint64* pIndex){
   COL_FUNCTION(LUAgetMessage);
   *pComponentId = LUAgetRegistryString(L, LUA_MESSAGE_INFO_COMPONENT);
   lua_getfield(L, LUA_REGISTRYINDEX, LUA_MESSAGE_INFO_DATETIME);
   *pDateTime = (COLuint64)lua_tointeger(L, -1);
   lua_getfield(L, LUA_REGISTRYINDEX, LUA_MESSAGE_INFO_ID);
   *pIndex = (COLuint64)lua_tointeger(L, -1);
   COL_TRC("Message data: ");
   COL_VAR3(*pComponentId, *pDateTime, *pIndex);
}

COLstring LUAgetMessageId(lua_State* L) {
   COL_FUNCTION(LUAgetMessageId);
   COLstring ComponentGuid;
   COLuint64 DateTimeMs = 0;
   COLuint64 Index = 0;
   COLstring RelatedId;
   LUAgetMessage(L, &ComponentGuid, &DateTimeMs, &Index);
   if (ComponentGuid.is_null()) { return ""; }
   return LUAserializeMessageId(ComponentGuid, DateTimeMs, Index);
}

bool LUAparseMessageId(const COLstring& Data, COLstring* pComponentId, COLuint64* pDateTime, COLuint64* pIndex) {
   COL_FUNCTION(LUAparseMessageId);
   COLstring Front, Back, Component;
   if (Data.is_null()) { return false; }
   if (!Data.rsplit(Front, Back, "-")) { return false; }
   *pIndex = atoll(Back.c_str());
   if (!Front.rsplit(Component, Back, "-")) { return false; }
   *pDateTime = atoll(Back.c_str());
   Component.swap(pComponentId);
   COL_VAR3(*pComponentId, *pDateTime, *pIndex);
   return true;
}

COLstring LUAserializeMessageId(const COLstring& ComponentId, COLuint64 DateTime, COLuint64 Index) {
   COL_FUNCTION(LUAserializeMessageId);
   COLstring Out;
   Out += ComponentId;
   Out += "-";
   Out += COLuint64ToString(DateTime);
   Out += "-";
   Out += COLuint64ToString(Index);
   return Out;
}