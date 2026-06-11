#ifndef __LCOM_LCOMISLIVE_H__
#define __LCOM_LCOMISLIVE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LCOMisLive
//
// Description: lua binding for component.live
//
// Author: Matthew Sobkowski
// Date:   Wednesday 26 April 2023 - 10:30AM
// ---------------------------------------------------------------------------
struct lua_State;

int LCOMisProductionMode(lua_State* L);

#endif // end of defensive include
