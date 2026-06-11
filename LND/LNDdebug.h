#ifndef __LND_DEBUG_H__
#define __LND_DEBUG_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LNDdebug
//
// Description:
//
// LNDdebug - this debug code used to be in LNDutils.  I want it separated out
// so that to make it easier to trace the debug code and not have it mixed in
// with the Lua NODe bindings.
//
// Author: Eliot Muir
// Date:   Thursday, October 28th, 2010 @ 09:55:16 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

struct lua_State;

class COLstring;
class LNDstackInfo;
class TRNannotationState;
class NODplace;

bool LNDincludeFunction(const NODplace* pPlace, const COLstring& FunctionName);

void LNDuserDataFormatter(lua_State* L, int i, COLostream& Stream);

// When set, NOD nodes will have extra information (their address) and callbacks
// will be executed when NOD nodes are assigned values.
void LNDsetNodDebug(lua_State* L);
bool LNDnodDebug(lua_State* L);

void LNDregisterNodeCallback(lua_State* L, TRNannotationState* pNodeCallback);
TRNannotationState* LNDgetNodeCallback(lua_State* L); // may return NULL - check LNDuseCallback() first.
void LNDclearNodeCallback(lua_State* L);


#endif // end of defensive include
