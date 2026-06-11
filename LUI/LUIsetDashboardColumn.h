#ifndef __LUI_SET_DASHBOARD_COLUMN_H__
#define __LUI_SET_DASHBOARD_COLUMN_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUIsetDashboardColumn
//
// Description:
//
// Lua binding for ui.setDashboardColumn (formerly component.setColumn). 
// Set custom columns to be loaded on the dashboard.
//
// Author: Vismay Shah 
// Date:   Thursday 17 April 2025 - 10:42AM
// ---------------------------------------------------------------------------
struct lua_State;

int LUIsetDashboardColumn(lua_State* L);

#endif // end of defensive include
