#ifndef __LUA_UD_FORMAT_H__
#define __LUA_UD_FORMAT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAudFormat
//
// Description:
//
// LUAudFormat - C function callback to print out user defined types
//
// Author: Eliot Muir
// Date:   Wednesday, June 23rd, 2010 @ 02:28:31 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

#include "LUAheader.h"

typedef void (*LUAudFormat)(lua_State* L, int StackIndex, COLostream& Stream); 

#endif // end of defensive include
