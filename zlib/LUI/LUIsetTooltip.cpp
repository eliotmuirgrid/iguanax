// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUIsetTooltip
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Thursday 17 April 2025 - 10:18AM
// ---------------------------------------------------------------------------
#include <LUI/LUIsetTooltip.h>

#include <LUA/LUAerror.h>
#include <LUA/LUAopen.h>
#include <LUA/LUAstartup.h>
#include <LUA/LUAtable.h>
#include <LUA/LUAtest.h>
#include <LUA/LUAutils.h>

#include <REX/REXutil.h>
#include <SCK/SCKcall.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

int LUIsetTooltip(lua_State* L) {
   COL_FUNCTION(LUIsetTooltip);
   COLstring HtmlTooltipBlock = LUAexpectedStringParam(L, 1, "data");
   bool      CompactHtml      = LUAoptionalBoolParam(L, 1, "compact", true);
   if(LUALisEnvironmentTest(L) || LUALisEnvironmentStartup(L)) {
      COL_TRC("IDE/Edit mode. no-op");
      return 0;
   }
   if (CompactHtml) { // Strip out any whitespace between tags
      HtmlTooltipBlock = REXreplace(HtmlTooltipBlock, ">\\s+<", "><");
   }
   COLvar Request;
   Request["component"] = LUAcomponentId(L);
   Request["html"] = HtmlTooltipBlock;
   COLvar Result;
   bool StatusSetResult = SCKcall("/component/tooltip/set", Request, LUAcomponentId(L), &Result);
   COL_VAR(Result);
   COL_VAR(StatusSetResult);
   if (!StatusSetResult || Result.exists("error")) { LUAraiseLuaError(L, "Failed to set component status html."); }
   COL_TRC("Back from sck call");
   return 0;
}
