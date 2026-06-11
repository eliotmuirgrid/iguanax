#ifndef __NODL_UTILS_H__
#define __NODL_UTILS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODLutils
//
// Description:
//
// NODLutils - utility functions.
//
// Author: Eliot Muir
// Date:   Thursday, October 14th, 2010 @ 12:48:53 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <COL/COLstring.h>
#include <LUA/LUAheader.h>

class NODplace;
class COLdateTime;
class COLvar;

void NODLutilsRegisterTypes();
// Protocol id for displaying nodes.
extern const COLstring NODLluaProtocolName;
extern const int NODL_LUA_PROTOCOL;

extern const COLstring NODLnullDisplayType;
extern const COLstring NODLunknownDisplayType;
extern const COLstring NODLerrorDisplayType;
extern const COLstring NODLwarningDisplayType;

// These IDs must not intersect with lua type IDs
enum NODLspecialDisplayTypes{
   NODL_JSON_NULL_DISPLAY_TYPE = 100,
   NODL_LUA_UNKNOWN_TYPE = 101,
   NODL_ERROR_DISPLAY_TYPE = 102,
   NODL_WARNING_DISPLAY_TYPE = 103
};

void NODLaddMethod(lua_State* L, const char* pName, lua_CFunction pFunction);
void NODLaddConstant(lua_State* L, const char* pName, int Value);
void NODLaddConstant(lua_State* L, const char* pName, const COLstring& Value);
bool COLvarIsVector(const COLvar& TheVar);

double      NODLoptionalDoubleParam(lua_State* L, int i, const COLstring& Param, double           DefaultValue);

bool NODLhasIntParam   (lua_State* L, int i, const COLstring& Param);
bool NODLhasBoolParam  (lua_State* L, int i, const COLstring& Param);
bool NODLhasDoubleParam(lua_State* L, int i, const COLstring& Param);
bool NODLhasStringParam(lua_State* L, int i, const COLstring& Param);


// As with tables above, but for function values.
bool NODLhasFuncParam     (lua_State* L, int i, const COLstring& Param);
void NODLexpectedFuncParam(lua_State* L, int i, const COLstring& Param);

int             NODLexpectedIntParam   (lua_State* L, int i, const COLstring& Param);
bool            NODLexpectedBoolParam  (lua_State* L, int i, const COLstring& Param);
double          NODLexpectedDoubleParam(lua_State* L, int i, const COLstring& Param);
const NODplace* NODLexpectedNodeParam  (lua_State* L, int i, const COLstring& Param);

// Returns an expected string parameter from the table at position i, or uses the parameter's
// __tostring metamethod, if available, to obtain a string. If the parameter is a number, 
// it will be coerced to a string.
COLstring       NODLexpectedStringParamFlexible(lua_State* L, int i, const COLstring& Param);

// NODLcheckParams verifies that the table at position i contains no keys
// except for those listed in the NULL terminated array aOptions.  If the
// table is missing or empty, pHelp is thrown; if the table contains a non-
// string key or one that is not listed in aOptions, an appropriate error
// is thrown with pHelp appended.
//
void NODLcheckParams(lua_State* L, int i, const char *const aOptions[]);

// "Extended" version of NODLcheckParams(). Use this when you want to coerce
// single-string-param functions called as "foo(param)" to
// foo{aOptions[0]=param}.
void NODLcheckParamsEx(lua_State* L, int i, const char *const aOptions[]);

//If there is one parameter for a connection-based method that isn't a table,
//replace with a table of parameters, and assign that parameter to ParamName in that table.
//Does a half-check that a connection is passed in - full checking done in getConnection().
void NODLdbConnectionParamToTable(lua_State* L, const char* ParamName);
const COLdateTime NODLconvertStringToDateTime(const COLstring& DateTimeAsString);

#endif // end of defensive include
