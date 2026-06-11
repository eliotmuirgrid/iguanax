//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LIGlog
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Thursday, Sept 29th, 2022
//---------------------------------------------------------------------------
#include "LIGlog.h"

#include <LUA/LUAutils.h>
#include <LUA/LUAstartup.h>
#include <LUA/LUAtest.h>
#include <LUA/LUAmessage.h>
#include <LUA/LUAopen.h>

#include <LUAL/LUALprint.h>
#include <LUAL/LUALutils.h>

#include <SCK/SCKloop.h>

#include <SDB/SDBcapi.h>

#include <LOG/LOGlog.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// MAIN THREAD
static void LUALlogMessage(COLauto<COLstring> Message, COLstring ChannelId) {
   COL_FUNCTION(LUALlogMessage);
   SDBIlogComponent(Message.get(), ChannelId);
}

int LUALprint(lua_State* L){
   COL_FUNCTION(LUALprint);
   if (LUALisEnvironmentTest(L) || LUALisEnvironmentStartup(L)) {
      COL_TRC("IDE/Edit mode. no-op");
      return 0;
   }
   COLstring Entry;
   COLostream Stream(Entry);
   int C = lua_gettop(L);
   if (C == 0) return 0;
   for (int i = 1; i <= C; i++) {
      if (i>1) Stream << ' ';
      LUALprintItem(L, i, Stream);
   }
   COL_VAR(Entry);
   COLauto<COLstring> Message = new COLstring();
   Entry.swap(Message);
   COLstring Guid = LUAcomponentId(L);
   SCKloop* pLoop = (SCKloop*)LUAgetPointerField(L, "_pEventLoop");
   pLoop->post(COLnewClosure0(&LUALlogMessage, Message, Guid));

   return 0;
}
