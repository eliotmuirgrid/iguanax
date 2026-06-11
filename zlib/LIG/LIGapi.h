#ifndef __LIG_API_H__
#define __LIG_API_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LIGapi
//
// Description:
//
// LIGapi is where the functions in the  "iguana" namespace for lua live.
//
// Author: Kevin Senn
// Date:   Tuesday, April 12th, 2011 @ 02:43:06 PM
//---------------------------------------------------------------------------
#include <COL/COLmap.h>

struct lua_State;

// specify Iguana version (static initialization, called once from main)
// takes effect when creating environments
void LIGspecifyIguanaVersion(int Major, int Minor, int Build);
void LIGapiAddIguanaNamespace(lua_State* L);
void LIGapiAddIguana6Namespace(lua_State* L);


#endif // end of defensive include
