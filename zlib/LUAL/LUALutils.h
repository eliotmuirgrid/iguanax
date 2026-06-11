#ifndef __LUAL_UTILS_H__
#define __LUAL_UTILS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2020 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUALutils
//
// Description:
//
// Utility functions for creating Lua C libraries that are dynamically loaded
// when needed. Copied from TNDLutils, mostly
//
// Author: John Q
// Date:   Thursday, October 14th, 2010 @ 12:48:53 PM
//
//---------------------------------------------------------------------------

class COLvar;
class COLstring;

#include <LUA/LUAheader.h>

// Protocol id for displaying nodes.
extern const COLstring LUALluaProtocolName;
extern const int LUAL_LUA_PROTOCOL;

extern const COLstring LUALnullDisplayType;
extern const COLstring LUALunknownDisplayType;
extern const COLstring LUALerrorDisplayType;
extern const COLstring LUALwarningDisplayType;


enum LUALspecialDisplayTypes{
   LUAL_JSON_NULL_DISPLAY_TYPE = 100,
   LUAL_LUA_UNKNOWN_TYPE = 101,
   LUAL_ERROR_DISPLAY_TYPE = 102,
   LUAL_WARNING_DISPLAY_TYPE = 103
};

// For exposing the int luaopen_<lib_name>(lua_State* L) function
#if defined(_WIN32) || defined(_WIN32)
#  define LUAL_DLL_EXPORT __declspec(dllexport)
#else
#  define LUAL_DLL_EXPORT __attribute__((visibility("default")))
#endif

// Macro for registering functions to the Lua table that is on the top of the
// Lua stack
#define LUAL_REGISTER_FUNCTION(_Name, _Function) lua_pushcfunction(L, _Function); lua_setfield(L, -2, _Name);

#define LUA_CATCH_INDEX(_Index)\
   catch (const COLerror& Error){\
   if (Error.code() == NOD_INDEX_OUT_OF_BOUNDS){\
      luaL_error(L, "Index %d is out of bounds.", _Index);\
   } else {\
      LUA_ERROR_STREAM(Error.description());\
   }\
   return 0;\
}

//Assumes a table or userdata is at the top of the stack.
//Gives it a metatable with a _submeta field, if there isn't one already.
//Sets _submeta[Name] to be the metatable called MetatableName.
//Exposes this metatable, and it is used by help system to document its __index functions.
void LUALaddSubMetatable(lua_State* L, const char* MetatableName, const char* Name);



// "Extended" version of LUAcheckParams(). Use this when you want to coerce
// single-string-param functions called as "foo(param)" to
// foo{aOptions[0]=param}.
void      LUAcheckParamsEx       (lua_State* L, int i, const char *const aOptions[]);

void LUAexpectedStringParamPushed(lua_State* L, int i, const COLstring& Param);

void LUALregisterLuaTypes();
#endif // end of defensive include
