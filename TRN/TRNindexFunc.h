#ifndef __TRN_INDEX_FUNC_H__
#define __TRN_INDEX_FUNC_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNindexFunc
//
// Description:
//
// TRNindexFunc find and wrap functions with routine to trace their inputs and outputs.
//
// Author: Eliot Muir
// Date:   Thursday, November 18th, 2010 @ 01:50:05 PM
//---------------------------------------------------------------------------
#include <COL/COLhashmap.h>
#include <LUA/LUAheader.h>

class TRNcfunction;

void TRNindexFuncGlobal(lua_State* L, COLhashmap<COLstring, TRNcfunction*>& CFunctionList, 
                        COLhashmap<const void*, COLstring, COLhashPointer<const void*> >& LFunctionNameMap);

#endif // end of defensive include
