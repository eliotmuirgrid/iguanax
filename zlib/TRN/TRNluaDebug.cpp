// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNluaDebug
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 07 June 2023 - 07:30AM
// ---------------------------------------------------------------------------

#include <TRN/TRNluaDebug.h>
#include <LUA/LUAheader.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

const char* TRNeventAsString(int eventNumber) {
    switch (eventNumber) {
        case LUA_HOOKCALL   : return "call";
        case LUA_HOOKRET    : return "return";
        case LUA_HOOKLINE   : return "line";
        case LUA_HOOKCOUNT  : return "count";
        case LUA_HOOKTAILRET: return "tail call";
        default: return "unknown";
    }
}

COLostream& operator<<(COLostream& Stream, const lua_Debug& D){
   Stream << "event = " << TRNeventAsString(D.event);

   //Stream << " name = " << Debug.name;
   return Stream;
}

COLstring TRNluaDebugName(lua_Debug* pD){
   COLstring Result;
   Result = COL_T("name = ") + pD->name + COL_T(", namewhat = ") + pD->namewhat;
   return Result;
}

COLstring TRNluaDebugSource(lua_Debug* pD){
   COLstring Result;
   Result = COL_T("source = ") + pD->source + COL_T(", short_src = ") + pD->short_src
          + COL_T(", linedefined = ") + COLintToString(pD->linedefined) + COL_T(", lastlinedefined = ")
          + COLintToString(pD->lastlinedefined) + COL_T(", what = ") + pD->what;
   return Result;
}
