#ifndef __TRN_ARGUMENT_POPULATE_CACHE_H__
#define __TRN_ARGUMENT_POPULATE_CACHE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNargumentPopulateCache
//
// Description:
//
// Functions which populate the argument cache for the annotation system.
//
// Author: Vismay Shah 
// Date:   Tuesday 02 May 2023 - 10:19AM
// ---------------------------------------------------------------------------
#include <COL/COLhashmap.h>
#include <COL/COLlist.h>
#include <COL/COLarray.h>
struct lua_State;
class TRNfunctionCall;
class TRNcfunction;
class TRNarg;
class TRNargRequest;
struct TRNreturnInfo;
class TRNframe;

void TRNargumentCachePopulateLuaCall(lua_State* L, COLlist<TRNframe>& Stack, COLlist<COLstring>& HashStack, COLhashmap<COLstring, TRNarg>& FullArgumentCache, COLarray<TRNargRequest>& FullArgumentRequests);

// This one is for C functions and returns
void TRNargumentCachePopulateCFunction(lua_State* L, TRNfunctionCall* pCaller, TRNcfunction* pCallee, int CallerLine, const COLstring& CalleeAddress, 
                                      COLhashmap<COLstring, TRNarg>& FullArgumentCache, COLarray<TRNargRequest>& FullArgumentRequests, bool IsReturn);

// This one handles lua function returns
void TRNargumentCachePopulateLuaReturn(lua_State* L, const TRNreturnInfo& ReturnInfo, const COLstring& CalleeAddress, TRNfunctionCall* pCaller, int CallerLine,
                                         TRNfunctionCall* pCallee, COLhashmap<COLstring, TRNarg>& FullArgumentCache, COLarray<TRNargRequest>& FullArgumentRequests);

#endif // end of defensive include
