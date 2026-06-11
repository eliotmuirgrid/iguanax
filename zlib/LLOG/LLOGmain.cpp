// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LLOGmain
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Thursday 17 April 2025 - 02:26PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <LLOG/LLOGmain.h>
#include <LOG/LOGlog.h>
#include <LUA/LUAcheck.h>
#include <LUA/LUAmessage.h>
#include <LUA/LUAopen.h>
#include <LUA/LUAstartup.h>
#include <LUA/LUAtable.h>
#include <LUA/LUAtest.h>
#include <LUA/LUAutils.h>
#include <SDB/SDBcapi.h>
COL_LOG_MODULE;

static int LLOGcheck(lua_State* L) {
   COL_FUNCTION(LLOGcheck);
   if(lua_gettop(L) < 1) {
      luaL_error(L, "Log text expected.");
      return 0;
   }
   if(LUALisEnvironmentTest(L) || LUALisEnvironmentStartup(L)) {
      COL_TRC("IDE/Edit mode. no-op");
      return 0;
   }
   return 1;
}

static int LLOGmain(lua_State* L, const COLstring& Message, const COLstring& Tags, const COLstring& RelatedId) {
   COL_FUNCTION(LLOGmain);
   COLstring Source;
   COLuint64 DateTime = 0, Index = 0;
   if(RelatedId.is_null()) {
      LUAgetMessage(L, &Source, &DateTime, &Index);   // This gets the source message that started the log to fill in the related messages data
   } else if(!LUAparseMessageId(RelatedId, &Source, &DateTime, &Index)) {
      return luaL_error(L, "Invalid message_id %s", RelatedId.c_str());
   }
   const COLstring Guid    = LUAcomponentId(L);
   const auto      Info    = LOGlogInfo(SDB_TRACE, Guid, Tags, Source, DateTime, Index);
   COL_VAR4(Info.m_Type, Info.m_ComponentId, Info.m_HashTags, Info.m_Source);
   LOGlog(Info, Message);
   return 0;
}

int LLOGregular(lua_State* L) {
   COL_FUNCTION(LLOGregular);
   if(!LLOGcheck(L)) { return 0; }
   COLstring Message, Tags, RelatedId;
   if(lua_isstring(L, 1)) {
      COL_TRC("Have string");
      Message   = LUAtoString(L, 1);
      RelatedId = luaL_optstring(L, 2, "");
      Tags      = luaL_optstring(L, 3, "");
   } else if(lua_istable(L, 1)) {
      COL_TRC("Have table");
      Message   = LUAoptionalStringParam(L, 1, "data", "");
      Tags      = LUAoptionalStringParam(L, 1, "tags", "");
      RelatedId = LUAoptionalStringParam(L, 1, "related_id", "");
   } else {
      COL_TRC("Have neither");
      luaL_error(L, "Parameter must be string or table.");
      return 0;
   }
   return LLOGmain(L, Message, Tags, RelatedId);
}

static int LLOGhelper(lua_State* L, const COLstring& Tag = "") {
   if (!LLOGcheck(L) || !lua_isstring(L, 1)) { return 0; }
   return LLOGmain(L, LUAtoString(L, 1), Tag, luaL_optstring(L, 2, ""));
}

int LLOGinfo(lua_State* L){
   COL_FUNCTION(LLOGinfo);
   return LLOGhelper(L, "#info");
}

int LLOGwarning(lua_State* L){
   COL_FUNCTION(LLOGwarning);
   return LLOGhelper(L, "#warning");
}

int LLOGerror(lua_State* L){
   COL_FUNCTION(LLOGerror);
   return LLOGhelper(L, "#error");
}

int LLOGdebug(lua_State* L){
   COL_FUNCTION(LLOGdebug);
   return LLOGhelper(L, "#debug");
}

int LLOGmessage(lua_State* L){
   COL_FUNCTION(LLOGmessage);
   return LLOGhelper(L);
}