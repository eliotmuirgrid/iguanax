#ifndef __TRN_HTML_CREATE_FUNCTION_CALL_H__
#define __TRN_HTML_CREATE_FUNCTION_CALL_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNhtmlCreateFunctionCall
//
// Description:
//
// Function I removed from TRNannotationState for easier tracing.
//
// Author: Eliot Muir 
// Date:   Wednesday 07 June 2023 - 08:59AM
// ---------------------------------------------------------------------------

struct lua_State;
class TRNfunctionCall;
class COLstring;

void TRNhtmlCreateFunctionCall(lua_State* L, int CountOfParam, TRNfunctionCall* pCaller, TRNfunctionCall* pCallee, int CallerLine, const COLstring& CalleeAddress);

#endif // end of defensive include
