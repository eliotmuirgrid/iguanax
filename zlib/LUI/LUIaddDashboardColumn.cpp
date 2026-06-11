// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUIaddDashboardColumn
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Thursday 17 April 2025 - 10:51AM
// ---------------------------------------------------------------------------
#include <LUI/LUIaddDashboardColumn.h>

#include <ICOL/ICOLmap.h>
#include <LUA/LUAcheck.h>
#include <LUA/LUAtable.h>
#include <LUA/LUAtest.h>
#include <LUA/LUAutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

int LUIaddDashboardColumn(lua_State* L) {
   COL_FUNCTION(LUIaddDashboardColumn);
   if(LUALisEnvironmentTest(L)) {
      COL_TRC("IDE mode. no-op.");
      return 0;
   }
   LUAcheckArgCount(L, 1, -1);
   LUAparamIsTable(L, 1);
   const COLstring Name     = LUAexpectedStringParam(L, 1, "name");
   const int       Width    = LUAoptionalIntParam(L, 1, "width", 40);
   const int       Priority = LUAoptionalIntParam(L, 1, "priority", 0);
   const COLstring Title    = LUAoptionalStringParam(L, 1, "title");
   ICOLadd(Name, Width, Priority, Title);
   return 0;
}