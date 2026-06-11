//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LIGapi
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Tuesday, April 12th, 2011 @ 02:43:06 PM
//
//---------------------------------------------------------------------------
#include "LIGapi.h"

#include <BLUA/BLUAcore.h>
#include <BLUA/BLUAinstance.h>
#include <COL/COLlog.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <LCOM/LCOMgetProjectDir.h>
#include <LCOM/LCOMgetId.h>
#include <LCOM/LCOMisTestMode.h>
#include <LCOM/LCOMsetStopOnError.h>
#include <LIG/LIGcall.h>
#include <LIG/LIGcallComponentFunction.h>
#include <LIG/LIGcomponents.h>
#include <LIG/LIGdate.h>
#include <LIG/LIGdifftime.h>
#include <LIG/LIGlog.h>
#include <LIG/LIGmessage.h>
#include <LIG/LIGtime.h>
#include <LIG/LIGregister.h>
#include <LUI/LUIaddDashboardColumn.h>
#include <LLOG/LLOGmain.h>
#include <LIG/LIGversion.h>
#include <LUA/LUAerror.h>
#include <LUA/LUAopen.h>
#include <LUA/LUAstartup.h>
#include <LUA/LUAtest.h>
#include <LUA/LUAtimeout.h>
#include <LUA/LUAutils.h>
#include <LUAL/LUALutils.h>
COL_LOG_MODULE;

static int LIGgetApplicationDir(lua_State* L){
   COL_FUNCTION(LIGgetApplicationDir);
   try{
      COLstring ExeDir = FILexecutableDir();
      FILaddPathSeparator(ExeDir);
      LUApushString(L,ExeDir);
      COL_VAR(ExeDir);
      return 1;
   }
   LUA_CATCH();
   return 0;
}

static int LIGgetWorkingDir(lua_State* L){
   COL_FUNCTION(LIGgetWorkingDir);
   try{
      COLstring WorkingDir = FILworkingDir();
      FILaddPathSeparator(WorkingDir);
      LUApushString(L, WorkingDir);
      COL_VAR(WorkingDir);
      return 1;
   }
   LUA_CATCH();
   return 0;
}

void LIGapiAddIguanaNamespace(lua_State* L){
   COL_FUNCTION(LIGapiAddIguanaNamespace);
   lua_newtable (L);
   LUAaddMethod(L, "appDir",            &LIGgetApplicationDir);
   LUAaddMethod(L, "translatorGuid",    &LCOMgetId);
   LUAaddMethod(L, "isTest",            &LCOMisTestMode);
   LUAaddMethod(L, "projectRoot",       &LCOMgetProjectDir);
   LUAaddMethod(L, "call",              &LIGcall);
   LUAaddMethod(L, "callComponentFunction", &LIGcallComponentFunction);
   LUAaddMethod(L, "callList",          &LIGcallList);
   LUAaddMethod(L, "setTimeout",        &LUAsetTimeout);
   LUAaddMethod(L, "logMessage",        &LLOGmessage);
   LUAaddMethod(L, "logError",          &LLOGerror);
   LUAaddMethod(L, "logWarning",        &LLOGwarning);
   LUAaddMethod(L, "logInfo",           &LLOGinfo);
   LUAaddMethod(L, "logDebug",          &LLOGdebug);
   LUAaddMethod(L, "log",               &LLOGregular);
   LUAaddMethod(L, "messageId",         &LIGmessageId);
   LUAaddMethod(L, "stopOnError",       &LCOMsetStopOnError);
   LUAaddMethod(L, "version",           &LIGversion);
   LUAaddMethod(L, "workingDir",        &LIGgetWorkingDir);
   LUAaddMethod(L, "components",        &LIGlistComponents);
   LUAaddMethod(L, "setColumn",         &LUIaddDashboardColumn);
   LUAaddMethod(L, "register",          &LIGregister);
   lua_setglobal(L, "iguana");
   // overwrite lua print to log message instead of print to stdout
   lua_pushcfunction(L, &LUALprint);
   lua_setglobal(L, "print");
}

void LIGapiAddIguana6Namespace(lua_State* L){
   COL_FUNCTION(LIGapiAddIguana6Namespace);
   lua_newtable(L);
   LUAaddMethod(L, "time",              &LIGtime);
   LUAaddMethod(L, "date",              &LIGdate);
   LUAaddMethod(L, "difftime",          &LIGdifftime);
   lua_setglobal(L, "iguana6");
}