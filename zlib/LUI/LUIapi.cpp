// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUIapi
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Thursday 17 April 2025 - 09:43AM
// ---------------------------------------------------------------------------
#include <LUI/LUIapi.h>
#include <LUI/LUIaddDashboardColumn.h>
#include <LUI/LUIsetBorderColor.h>
#include <LUI/LUIsetDashboardColumn.h>
#include <LUI/LUIsetStatusMessage.h>
#include <LUI/LUIsetTooltip.h>

#include <BLUA/BLUAinstance.h>
#include <LUA/LUAutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void LUInamespace(lua_State* L){
   COL_FUNCTION(LUInamespace);
   lua_newtable (L);
   LUAaddMethod(L, "setBorderColor",     &LUIsetBorderColor);
   LUAaddMethod(L, "setStatusLight",     &BLUAsetLight);
   LUAaddMethod(L, "setStatusMessage",   &LUIsetStatusMessage);
   LUAaddMethod(L, "setTooltip",         &LUIsetTooltip);
   LUAaddMethod(L, "setDashboardColumn", &LUIsetDashboardColumn);
   LUAaddMethod(L, "addDashboardColumn", &LUIaddDashboardColumn);
   lua_setglobal(L, "ui");
}
