#ifndef __LSTR_STRING_H_
#define __LSTR_STRING_H_
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LSTRstring
//
// Description:
//
// Defines some additional functionality on top of strings
//
// Author: Akshay Ganeshen
// Date:   Thu  6 Nov 2014 15:23:45 EST
//
//---------------------------------------------------------------------------
#include <LUA/LUAheader.h>

// Returns an array of strings
int LSTRsplit(lua_State* L);
// Note that this returns an iterator function, and not a string
int LSTRrxmatch(lua_State* L);
// Returns the substituted string (with up to max)
int LSTRrxsub(lua_State* L);
// Trims outer whitespace from string
int LSTRstringTrimWS(lua_State* L);
// Trims right whitespace from string
int LSTRstringTrimRWS(lua_State* L);
// Trims left whitespace from string
int LSTRstringTrimLWS(lua_State* L);
// Removes whitespace from string
int LSTRstringCompactWS(lua_State* L);
// Sets string to sentence case
int LSTRstringCapitalize(lua_State* L);

void LSTRaddStringNamespace(lua_State* L);

#endif // end of defensive include
