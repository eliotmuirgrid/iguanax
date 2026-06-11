#ifndef __LIG_CALL_COMPONENT_FUNCTION_H__
#define __LIG_CALL_COMPONENT_FUNCTION_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LIGcallComponentFunction
//
// Description:
// iguana.callComponentFunction
// Lua api which takes in a target component, function and a table of data
// and runs the specified function using data as the argument. Blocks until it
// returns and returns the result to the Lua state. Formerly component.call.
//
// Author: Vismay Shah
// Date:   Monday 21 April 2025 - 03:09PM
// ---------------------------------------------------------------------------
#include <COL/COLclosure.h>

class BLUAinstanceManager;
class SCKloop;
class COLvar;
struct CFGmap;
struct lua_State;

int LIGcallComponentFunction(lua_State* L);
void LIGsetUpComponentCall(lua_State* L, CFGmap* pMap, BLUAinstanceManager* pBluaManager, SCKloop* pLoop);

void LIGcomponentCallOnMain(COLvar* pData, CFGmap* pMap, BLUAinstanceManager* pManager, SCKloop* pLoop, COLclosure1<COLvar>* pCallback);

#endif // end of defensive include
