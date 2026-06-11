//---------------------------------------------------------------------------
// Copyright (C) 1997-2020 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAerror
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wed 31 Aug 2022 14:35:10 EDT
//---------------------------------------------------------------------------

#include <LUA/LUAerror.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

int LUAraiseLuaError(lua_State* L, const COLstring& ErrorString) {
   COL_FUNCTION(LUAraiseLuaError);
   COL_VAR(ErrorString);
   return luaL_error(L, "%s", ErrorString.c_str());
}
