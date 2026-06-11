#ifndef __TRN_CONNECTION_CHECK_H__
#define __TRN_CONNECTION_CHECK_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNconnectionCheck
//
// Description:
//
// TODO
//
// Author: Vismay Shah 
// Date:   Thursday 08 June 2023 - 01:45PM
// ---------------------------------------------------------------------------
struct lua_State;

#include <COL/COLminimumInclude.h>

void TRNsetConnectionIdInLua    (lua_State* L, COLuint64 ConnectionId);
COLuint64 TRNconnectionIdFromLua(lua_State* L);

void TRNconnectionCheckTimeRegister(lua_State* L);

// This function checks whether we still have a connection to the browser
// called as part of TRNeventDebugHook
bool TRNconnectionCheck            (lua_State* L);

#endif // end of defensive include
