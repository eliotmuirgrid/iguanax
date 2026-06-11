#ifndef __TRN_ANNOTATION_C_FUNCTION_H__
#define __TRN_ANNOTATION_C_FUNCTION_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNannotationCFunction
//
// Description:
//
// C function related code for the annotation system
//
// Author: Vismay Shah 
// Date:   Wednesday 03 May 2023 - 03:44PM
// ---------------------------------------------------------------------------

#include <COL/COLhashmap.h>
#include <COL/COLarray.h>
#include <TRN/TRNargumentRequest.h>
#include <TRN/TRNannotationState.h>

class TRNfunctionCall;
class TRNcfunction;

void TRNannotationCFunction(lua_State* L, TRNfunctionCall* pCaller, TRNcfunction* pCallee, int CallerLine, const COLstring& CalleeAddress, COLhashmap<COLstring, TRNarg>& FullArgumentCache, COLarray<TRNargRequest>& FullArgumentRequests);
void TRNannotationCReturn  (lua_State* L, TRNfunctionCall* pCaller, TRNcfunction* pCallee, int CallerLine, const COLstring& CalleeAddress, COLhashmap<COLstring, TRNarg>& FullArgumentCache, COLarray<TRNargRequest>& FullArgumentRequests);
void TRNannotationUntracedCFunction(lua_State* L, lua_Debug* pD, COLlist<TRNframe>& Stack);

TRNcfunction* TRNaddCFunction(COLhashmap<COLstring, TRNcfunction*>& CFunctionList, const COLstring& Name);
void TRNcleanUpCFunctions(COLhashmap<COLstring, TRNcfunction*>& List);

#endif // end of defensive include
