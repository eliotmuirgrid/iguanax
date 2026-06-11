#ifndef __LIV_API_H__
#define __LIV_API_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LIVapi
//
// Description:
//
// Wraps around iconv library to provide its functions in the translator
//
// All functions add as an encode filter, translating various encoding types
// to UTF-8 (the preferred format for a lot of the other library code)
// The available encoders/decoders are calculated dynamically with a query
// to TXT, so each platform may have different character encodings available
//
// Author: Akshay Ganeshen
// Date:   Mon 24 Nov 2014 09:49:37 EST
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <LUA/LUAheader.h>

// iconv, available as a function applied to data, from_encoding, to_encoding
int LIViconvConvert(lua_State* L);

// This allows querying iconv for the list of encodings it supports
int LIViconvListEncodings(lua_State* L);
// and this allows getting the list of aliases for a particular encoding
int LIViconvListAliases(lua_State* L);

int LIViconvIsSupported(lua_State* L);

// Handler that dynamically figures out available encodings and registers
// them into the current table (on the stack)
void LIVaddIconvNamespace(lua_State* L);

#endif // end of defensive include
