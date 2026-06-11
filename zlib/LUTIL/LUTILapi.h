#ifndef __LUTIL_API_H__
#define __LUTIL_API_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUTILapi
//
// Description:
//
// LUTILapi is where the functions in the "util" namespace for lua live.
//
// Author: Kevin Senn
// Date:   Tuesday, April 12th, 2011 @ 02:43:06 PM
//
//---------------------------------------------------------------------------
struct lua_State;
void LUTILapiAddUtilNamespace(lua_State* L);

#endif // end of defensive include
