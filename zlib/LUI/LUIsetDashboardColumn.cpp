// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUIsetDashboardColumn
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Thursday 17 April 2025 - 10:42AM
// ---------------------------------------------------------------------------
#include <LUI/LUIsetDashboardColumn.h>

#include <ICOL/ICOLcomponent.h>
#include <LUA/LUAcheck.h>
#include <LUA/LUAopen.h>
#include <LUA/LUAtable.h>
#include <LUA/LUAtest.h>
#include <LUA/LUAutils.h>
#include <SCK/SCKloop.h>
#include <COL/COLclosure.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

int LUIsetDashboardColumn(lua_State* L) {
   COL_FUNCTION(LUIsetDashboardColumn);
   if(LUALisEnvironmentTest(L)) {
      COL_TRC("IDE mode. no-op.");
      return 0;
   }
   LUAcheckArgCount(L, 1, -1);
   LUAparamIsTable(L, 1);
   const COLstring Component = LUAcomponentId(L);
   const COLstring Name      = LUAexpectedStringParam(L, 1, "name");
   const COLstring Value     = LUAexpectedStringParam(L, 1, "value");
   SCKloop* pLoop      = (SCKloop*)LUAgetPointerField(L, "_pEventLoop");
   pLoop->post(COLnewClosure0(&ICOLcomponentAdd, Component, Name, Value));
   return 0;
}