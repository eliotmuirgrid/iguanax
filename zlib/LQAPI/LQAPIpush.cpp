//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LQAPIpush
//
// Description: The "queue" api for Lua.
//
//
// Author: Vismay Shah
// Date:   Tues, June 7th, 2021
//
//---------------------------------------------------------------------------
#include "LQAPIpush.h"

#include <BLUA/BLUAinstance.h>
#include <BLUA/BLUAqueue.h>
#include <BLUA/BLUAtimer.h>

#include <LUA/LUAheader.h>
#include <LUA/LUAutils.h>
#include <LUA/LUAstartup.h>
#include <LUA/LUAtable.h>
#include <LUA/LUAtest.h>
#include <LUA/LUAopen.h>
#include <LUA/LUAmessage.h>

#include <SCK/SCKloop.h>
#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>

#include <COL/COLlocker.h>
#include <COL/COLarray.h>
#include <COL/COLmap.h>
#include <COL/COLmutex.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// queue.push{data=<msg>} or queue.push(data)
int LQAPIpush(lua_State* L){
   COL_FUNCTION(LQAPIpush);
   COLauto<BLUAenqueueData> pData = new BLUAenqueueData();
   if (lua_istable(L, 1)) {   // queue.push{data="MSH", tags="#fhir", meta="hl7"}
      pData->Data = LUAexpectedStringParam(L, 1, "data");
      pData->Tags = LUAoptionalStringParam(L, 1, "tags");
      if (LUAoptionalTableParam(L, 1, "meta")) {
         try {
            pData->Meta.fromLua(L);
         } catch (const COLerror& Error) {
            return luaL_error(L, Error.description().c_str());
         }
      }
   } else {                   // queue.push("MSH", "#fhir")
      size_t len = 0;
      pData->Data = luaL_checklstring(L, 1, &len);
      if (lua_gettop(L) > 1) {
         pData->Tags = luaL_checklstring(L, 2, &len);
         // TODO: meta support
      }
   }
   if (LUALisEnvironmentTest(L) || LUALisEnvironmentStartup(L)) {
      COL_TRC("IDE mode - return 0-0-0");
      LUApushString(L, "0-0-0");
      return 1;
   }
   LUAgetMessage(L, &pData->RelatedId.ComponentId, &pData->RelatedId.DateTime, &pData->RelatedId.Index);
   COL_TRC("Enqueueing message.");
   COLstring Error;
   BLUAinstance2* pInstance = BLUAinstanceGet(L);
   COLstring Id = BLUAinstanceEnqueue(pInstance, pData, &Error);  // a blocking call
   if (Id.is_null()){
      COL_TRC("Have a null message id - return nil to lua");
      if (Error.is_null()) {
         lua_pushnil(L);
         return 1;
      } else {
         COL_TRC("Enqueue failed with error: " << Error);
         return luaL_error(L, Error.c_str());
      }
   }
   COL_TRC("Have valid message id: " << Id);
   LUApushString(L, Id);
   return 1;
}