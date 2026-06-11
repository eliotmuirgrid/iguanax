#ifndef __LUA_MESSAGE_H__
#define __LUA_MESSAGE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAmessage
//
// Description:
//
// TODO
//
// Author: Vismay Shah
// Date:   Tuesday 26 September 2023 - 12:43PM
// ---------------------------------------------------------------------------
#include <COL/COLminimumInclude.h>
struct lua_State;
class COLstring;

void LUAsetupDefaultMessage(lua_State* L);

void LUAsetMessage(lua_State* L, const COLstring& ComponentId, COLuint64 DateTime, COLuint64 Index);
void LUAgetMessage(lua_State* L, COLstring* pComponentId, COLuint64* pDateTime, COLuint64* pIndex);

// returns the stringified message id stored in the Lua state.
// an empty string is returned if the message id is not set.
COLstring LUAgetMessageId(lua_State* L);

bool LUAparseMessageId(const COLstring& Data, COLstring* pComponentId, COLuint64* pDateTime, COLuint64* pIndex);
COLstring LUAserializeMessageId(const COLstring& ComponentId, COLuint64 DateTime, COLuint64 Index);

#endif // end of defensive include
