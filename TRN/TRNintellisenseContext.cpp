//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNintellisenseContext
//
// Description:
//
// TRNintellisenseContext - class refactored from NWB intellisense code.
//
// Author: Eliot Muir and Kevin Senn
// Date:   Monday, December 20th, 2010 @ 01:25:57 PM
//---------------------------------------------------------------------------
#include <TRN/TRNintellisenseContext.h>

#include <LUAL/LUALutils.h>
#include <LUA/LUAutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

const COLstring TRN_INTELLISENSE_CONTEXT_REGISTRY_NAME = "IntelReg";

TRNintellisenseContext* TRNintellisenseContextGet(lua_State* L){
   COL_FUNCTION(TRNintellisenseContextGet);
   LUApushString(L, TRN_INTELLISENSE_CONTEXT_REGISTRY_NAME);
   lua_gettable(L, LUA_REGISTRYINDEX);
   TRNintellisenseContext* pContext = (TRNintellisenseContext*)lua_touserdata(L, -1);
   return pContext;  
}

void TRNintellisenseContextSet(lua_State* L, TRNintellisenseContext* pContext){
   COL_FUNCTION(TRNintellisenseContextSet);
   LUApushString(L, TRN_INTELLISENSE_CONTEXT_REGISTRY_NAME);
   lua_pushlightuserdata(L, (void*)pContext);
   lua_settable(L, LUA_REGISTRYINDEX);
}

COLostream& operator<<(COLostream& Stream, const TRNintellisenseContext& Object){
   Stream << "Intellisense Context file=" << Object.CurrentFile
                      << " line=" << Object.LineNumber
                      << " cursor=" << Object.ParseContext.m_Cursor
                      << " full_expression=" << Object.ParseContext.FullExpression;
   return Stream;
}
