#ifndef __LJSON_API_H__
#define __LJSON_API_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LJSONapi
//
// Description:
//
// LJSONapi is where the functions in the "json" namespace for lua live.
//
// Author: Kevin Senn
// Date:   Tuesday, April 12th, 2011 @ 02:43:06 PM
//
//---------------------------------------------------------------------------
struct lua_State;

extern const char* LJSON_NAMESPACE;
void LJSONaddJsonConstants(lua_State* L);
int LJSONjsonParseVar(lua_State* L);
int LJSONnativeJsonSerialize(lua_State* L);
int LJSONcreateEmptyJsonObject(lua_State* L);

void LJSONapiAddJsonNamespace(lua_State* L);

#endif // end of defensive include
