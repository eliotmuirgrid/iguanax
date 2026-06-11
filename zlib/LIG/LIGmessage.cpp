// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LIGmessage
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Tuesday 17 October 2023 - 04:47PM
// ---------------------------------------------------------------------------

#include <LIG/LIGmessage.h>
#include <LUAL/LUALutils.h>
#include <LUA/LUAmessage.h>
#include <LUA/LUAstartup.h>
#include <LUA/LUAtest.h>
#include <LUA/LUAutils.h>
#include <LUA/LUAopen.h>

#include <CFG/CFGmap.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static COLstring LIGserializeMessageId(lua_State* L){
   COL_FUNCTION(LIGserializeMessageId);
   COLstring ComponentGuid;
   COLuint64 DateTimeMs = 0;
   COLuint64 Index = 0;
   LUAgetMessage(L, &ComponentGuid, &DateTimeMs, &Index);
   if (ComponentGuid.is_null()) {
      COL_TRC("Component guid not defined - we don't have a valid message id.");
      return "";
   }
   COLstring Out = LUAserializeMessageId(ComponentGuid, DateTimeMs, Index);
   COL_VAR(Out);
   return Out;
}

static int LIGreturnNil(lua_State* L) {
   lua_pushnil(L);
   return 1;
}

static int LIGreturnId(lua_State* L, const COLstring& Message) {
   LUApushString(L, Message);
   return 1;
}

int LIGmessageId(lua_State* L) {
   COL_FUNCTION(LIGmessageId);
   if (LUALisEnvironmentTest(L) || LUALisEnvironmentStartup(L)) {
      COL_TRC("IDE mode");
      COLstring ComponentId = LUAcomponentId(L);
      CFGmap* pNodeConfigs = (CFGmap*) LUAgetPointerField(L, "_pConfigMap");
      bool IsSource = pNodeConfigs->ConfigMap[ComponentId].CoreConfig.Sources.size() == 0;
      COL_VAR(IsSource);
      return IsSource ? LIGreturnNil(L) : LIGreturnId(L, "0-0-0");
   }
   COLstring Message = LIGserializeMessageId(L);
   COL_VAR(Message);
   return Message.is_null() ? LIGreturnNil(L) : LIGreturnId(L, Message);
}
