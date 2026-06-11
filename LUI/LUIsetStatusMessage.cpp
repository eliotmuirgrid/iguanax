// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUIsetStatusMessage
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Thursday 17 April 2025 - 10:01AM
// ---------------------------------------------------------------------------
#include <LUI/LUIsetStatusMessage.h>

#include <LUA/LUAerror.h>
#include <LUA/LUAopen.h>
#include <LUA/LUAstartup.h>
#include <LUA/LUAtable.h>
#include <LUA/LUAtest.h>
#include <LUA/LUAutils.h>

#include <SCK/SCKcall.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

int LUIsetStatusMessage(lua_State* L) {
   COL_FUNCTION(LUIsetStatusMessage);
   COLstring HtmlBlock = LUAexpectedStringParam(L, 1, "data");
   if(LUALisEnvironmentTest(L) || LUALisEnvironmentStartup(L)) {
      COL_TRC("IDE/Edit mode. no-op");
      return 0;
   }
   COLvar Request;
   Request["component"] = LUAcomponentId(L);
   Request["html"] = HtmlBlock;
   COLvar Result;
   bool StatusSetResult = SCKcall("/component/status/set", Request, LUAcomponentId(L), &Result);
   COL_VAR(Result);
   COL_VAR(StatusSetResult);
   if (!StatusSetResult || Result.exists("error")) { LUAraiseLuaError(L, "Failed to set component status html."); }
   COL_TRC("Back from sck call");
   return 0;
}