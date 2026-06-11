#ifndef __LNET_RESPOND_H__
#define __LNET_RESPOND_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LNETrespond
//
// Description:
//
// Lua binding for net.http.respond.
// Responds to the current http connection being handled in the Lua script.
//
// Author: Vismay Shah
// Date:   Friday 27 October 2023 - 03:25PM
// ---------------------------------------------------------------------------
struct lua_State;
class  COLstring;
class  COLvar;
class  BLUAinstance2;

int LNEThttpSendResponse(lua_State* L);

// internal
int LNEThttpRespondErrorLua(lua_State* L, const COLstring& ErrorMessage);
bool LNEThttp2RespondAsync(BLUAinstance2* pInstance, const COLvar& Parameters, int* pConnId, int* pStreamId, COLstring* pError);

#endif // end of defensive include
