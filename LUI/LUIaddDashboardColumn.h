#ifndef __LUI_ADD_DASHBOARD_COLUMN_H__
#define __LUI_ADD_DASHBOARD_COLUMN_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUIaddDashboardColumn
//
// Description:
//
// Lua binding for ui.addDashboardColumn (formerly iguana.setColumn). 
// Set custom columns to be loaded on the dashboard.
//
// Author: Vismay Shah 
// Date:   Thursday 17 April 2025 - 10:51AM
// ---------------------------------------------------------------------------
struct lua_State;

int LUIaddDashboardColumn(lua_State* L);

#endif // end of defensive include
