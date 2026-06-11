#ifndef __LUA_ERROR_H__
#define __LUA_ERROR_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAerror
//
// Description:
//
// Helper functions for error handling
//
// Author: Eliot Muir
// Date:   Wed 31 Aug 2022 14:35:10 EDT
//---------------------------------------------------------------------------

#include <LUA/LUAheader.h>

class COLstring;

// Catching exceptions
int LUAraiseLuaError(lua_State* L, const COLstring& ErrorString);
#define LUA_ERROR_STREAM(_Message) { COLstring _ErrorString; COLostream _ErrorStream(_ErrorString); _ErrorStream << _Message; LUAraiseLuaError(L, _ErrorString);}
#define LUA_CATCH()\
   catch (const COLerror& Error){\
   LUA_ERROR_STREAM(Error.description());\
   return 0;\
}

#endif // end of defensive include
