#ifndef __LLOG_MAIN_H__
#define __LLOG_MAIN_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LLOGmain
//
// Description:
// log Translator APIs.
// formerly iguana.log* apis.
//
// Author: Vismay Shah
// Date:   Thursday 17 April 2025 - 02:26PM
// ---------------------------------------------------------------------------
struct lua_State;

int LLOGregular(lua_State* L);
int LLOGinfo(lua_State* L);
int LLOGwarning(lua_State* L);
int LLOGerror(lua_State* L);
int LLOGdebug(lua_State* L);
int LLOGmessage(lua_State* L);

#endif // end of defensive include
