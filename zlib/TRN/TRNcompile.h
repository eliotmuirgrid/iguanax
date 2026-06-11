#ifndef __TRN_COMPILE_H__
#define __TRN_COMPILE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNcompile
//
// Description:
//
// Compilation routines for translator.
//
// Author: Eliot Muir
// Date:   Wednesday, November 17th, 2010 @ 12:55:34 PM
//---------------------------------------------------------------------------
#include <LUA/LUAheader.h>

class TRNannotationState;
class COLstring;

void TRNcompileHookSet(lua_State* L);
void TRNcompileHookClear(lua_State* L);

void TRNdestroyLua(lua_State** pL);

void TRNcompile(lua_State** pL, TRNannotationState* pState, COLstring* pError);

void TRNcompileRedoWithLoopEnd(lua_State* L, const COLstring& Source, const COLstring& FileName);
bool TRNcompileDouble(lua_State* L, const COLstring& Source, const COLstring& FileName, COLstring* pError);

#endif // end of defensive include