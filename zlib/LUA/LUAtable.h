#ifndef __LUA_TABLE_H__
#define __LUA_TABLE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAtable
//
// Description:
//
// Helper functions to read parameters from tables.  Usually used in our C Lua library code.
//
// Author: Eliot Muir
// Date:   Wed 31 Aug 2022 14:35:10 EDT
//---------------------------------------------------------------------------

#include <LUA/LUAheader.h>
#include <COL/COLstring.h>

bool LUApushParamByName(lua_State* L, int i, const COLstring& Param);
void LUAexpectedStringParamPushed(lua_State* L, int i, const COLstring& Param);

void      LUAparamIsTable(lua_State* L, int i);

COLstring LUAexpectedStringParam(lua_State* L, int i, const COLstring& Param);
int       LUAexpectedIntParam   (lua_State* L, int i, const COLstring& Param);
void      LUAexpectedTableParam (lua_State* L, int i, const COLstring& Param);
void      LUAexpectedFuncParam  (lua_State* L, int i, const COLstring& Param);
bool      LUAexpectedBoolParam  (lua_State* L, int i, const COLstring& Param);

COLstring LUAoptionalStringParam(lua_State* L, int i, const COLstring& Param, const COLstring& DefaultValue = "");
bool      LUAoptionalBoolParam  (lua_State* L, int i, const COLstring& Param, bool DefaultValue = false);
int       LUAoptionalIntParam   (lua_State* L, int i, const COLstring& Param, int DefaultValue);
double    LUAoptionalDoubleParam(lua_State* L, int i, const COLstring& Param, double DefaultValue);
bool      LUAoptionalTableParam (lua_State* L, int i, const COLstring& Param);
bool      LUAoptionalFuncParam  (lua_State* L, int i, const COLstring& Param);

bool LUAoptionalStringParamPushed(lua_State* L, int i, const COLstring& Param);

#endif // end of defensive include