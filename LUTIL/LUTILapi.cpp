//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUTILapi
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Tuesday, April 12th, 2011 @ 02:43:06 PM
//
//---------------------------------------------------------------------------
#include "LUTILapi.h"

#include <BLUA/BLUAinstance.h>
#include <BLUA/BLUAsleep.h>
#include <COL/COLerror.h>
#include <COL/COLlog.h>
#include <COL/COLutils.h> // for sleep
#include <LUA/LUAutils.h>
#include <LUA/LUAerror.h>
#include <LUA/LUAcheck.h>
#include <LHLP/LHLPmapMethods.h>
#include <LUTIL/LUTILfindPattern.h>
#include <LUTIL/LUTILsafeCall.h>
#include <LUTIL/LUTILsleep3.h> // timer based sleep implementation for translator

#include <LUAL/LUALutils.h>
#include <LUA/LUAtimeout.h>
#include <SFI/SFIguid.h>
#include <SFI/SFImd5.h>
COL_LOG_MODULE;

static int LUTILgenerateGuid(lua_State* L){
   COL_FUNCTION(LUTILgenerateGuid);
   LUAcheckArgCount(L, 1, -1);
   int BitStrength = (int)lua_tointeger(L,1);
   if (BitStrength < 128 || (BitStrength % 8) != 0){
      LUA_ERROR_STREAM(COL_T("size argument must be at least 128 and should be divisible by 8"));
   }
   try{
      COLstring GUIDstring = SFIgenerateGUID(BitStrength);
      LUApushString(L, GUIDstring);
      COL_VAR(GUIDstring);
      return 1;
   }LUA_CATCH();
}

static int LUTILmd5(lua_State* L){
   COL_FUNCTION(LUTILmd5);
   LUAcheckArgCount(L, 1, -1);
   try{
      COLstring Data;
      if (!LUAextractString(L, 1, &Data)) {
         luaL_error(L, "Argument must be a string or a node tree.");
      }
      LUApushString(L, SFImd5(Data));
      return 1;
   }LUA_CATCH();
}

#define LUTIL_SLEEP_INCREMENT 1000

/*static int LUTILsleep(lua_State* L){
   COL_FUNCTION(LUTILsleep);
   LUAcheckArgCount(L, 1, -1);
   try{
      int TimeToSleep = lua_tointeger(L, 1);
      if (TimeToSleep < 0){
         LUA_ERROR_STREAM(COL_T("sleep must be given a positive argument."));
      }

      // If in production mode and sleep for more than two seconds, use the
      // suspend plus timer method so sleep doesn't interfere with shutdown.
      BLUAinstance2* pInstance = BLUAinstanceGet(L);
      if (TimeToSleep > 2000 && pInstance) {
         COL_TRC("Sleep with timer...");
         BLUAinstanceSleep(pInstance, TimeToSleep);
         return 0;
      }

      // We'll sleep for at most LUTIL_SLEEP_INCREMENT ms at a time,
      // so that we can still detect timeouts.
      COL_VAR(TimeToSleep);
      while (TimeToSleep > 0){
         COL_TRC("Going to sleep...");
         COLsleep(COL_MIN(TimeToSleep, LUTIL_SLEEP_INCREMENT));
         LUAcheckTimeout(L);
         TimeToSleep -= LUTIL_SLEEP_INCREMENT;
      }
      return 0;
   }LUA_CATCH();
}*/

void LUTILapiAddUtilNamespace(lua_State* L){
   COL_FUNCTION(LUTILapiAddUtilNamespace);
   lua_newtable (L);
   LUAaddMethod(L, "findPattern",  &LUTILfindPattern);
   LUAaddMethod(L, "guid",         &LUTILgenerateGuid);
   LUAaddMethod(L, "generateGUID", &LUTILgenerateGuid);
   LUAaddMethod(L, "md5",          &LUTILmd5);
   LUAaddMethod(L, "safeCall",     &LUTILsafeCall);
   //LUAaddMethod(L, "sleep", &LUTILsleep);
   LUAaddMethod(L, "sleep", &LUTILsleep3);
   lua_setglobal(L, "util");
}