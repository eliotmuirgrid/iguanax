#ifndef __TRN_ANNOTATION_LUA_FUNCTION_H__
#define __TRN_ANNOTATION_LUA_FUNCTION_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNannotationLuaFunction
//
// Description:
//
// Lua function related code for the annotation system
//
// Author: Vismay Shah 
// Date:   Wednesday 03 May 2023 - 01:50PM
// ---------------------------------------------------------------------------
#include <COL/COLlist.h>
#include <COL/COLhashmap.h>
#include <LUA/LUAheader.h>
#include <TRN/TRNargumentRequest.h>
#include <TRN/TRNframe.h>

class TRNfile;
class TRNfunction;
struct TRNreturnInfo;

// This one gets the function details and sets up a TRNframe
void TRNsetupLFunctionFrame(TRNfunction* pCallee, lua_State* L, lua_Debug* pD, const char* pSource, COLlist<TRNframe>& Stack, COLlist<COLstring>& HashStack,
                         COLhashmap<COLstring, TRNfile*>& LuaFileList, COLhashmap<const void*, COLstring, COLhashPointer<const void*> >& LFunctionNameMap, 
                         COLstring* pCurrentAddress, int& LastCallStackDepth);  // stack has parameters, but we're not sure how many

// This one sets the function call arguments to the stack frame and then calls the function to generate the annotation for it
void TRNannotationLuaCall(lua_State* L, int CountOfParam, int FirstBodyLine, COLlist<TRNframe>& Stack, COLlist<COLstring>& HashStack, 
                          int LastCallStackDepth);  // stack has parameters
void TRNannotationLuaReturn(lua_State* L, lua_Debug* pD, COLlist<TRNframe>& Stack, COLlist<COLstring>& HashStack, bool& IgnoreNextReturn, 
                  int LastCallStackDepth, TRNreturnInfo* pMostRecentReturnInfo, COLstring* pCurrentAddress, COLhashmap<COLstring, TRNarg>& FullArgumentCache, COLarray<TRNargRequest>& FullArgRequests); // return values in locals or stack, depending on Lua implementation.

void TRNhtmlLuaFunctionReturn(lua_State* L, const TRNreturnInfo& ReturnInfo, TRNfunctionCall* pCaller, TRNfunctionCall* pCallee, int CallerLine, int CalleeLine, const COLstring& FunctionAddress);

// Functions for accessing the function pointer -> name map for global Lua functions.
// This is just to name the functions nicely even if they're called anonymously.
COLhashmapPlace  TRNfindLuaFunctionName(COLhashmap<const void*, COLstring, COLhashPointer<const void*> >& LFunctionNameMap, const void* pFunction);
const COLstring& TRNluaFunctionName    (COLhashmap<const void*, COLstring, COLhashPointer<const void*> >& LFunctionNameMap, COLhashmapPlace FunctionPlace);
void             TRNsetLuaFunctionName (COLhashmap<const void*, COLstring, COLhashPointer<const void*> >& LFunctionNameMap, const void* pFunction, const COLstring& FunctionName);
#endif // end of defensive include