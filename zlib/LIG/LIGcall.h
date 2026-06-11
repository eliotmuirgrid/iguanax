#ifndef __LIG_CALL_H__
#define __LIG_CALL_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2020 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LIGcall
//
// Description:
//
// LIGcall can be used to call an api on the local Iguana or a peer connected
// Iguana from a Lua script. 
//
// LIGcallList lists the apis available to be called.
//
// Author: Vismay Shah
//---------------------------------------------------------------------------
struct lua_State;
class COLstring;

void LIGpopulateAgent(lua_State* L, COLstring *pAgent, bool& ComponentAgent);
int LIGcallList(lua_State* L);
int LIGcall(lua_State* L);

#endif