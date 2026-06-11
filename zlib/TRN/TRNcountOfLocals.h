#ifndef __TRN_COUNT_OF_LOCALS_H__
#define __TRN_COUNT_OF_LOCALS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNcountOfLocals
//
// Description:
//
// Helper
//
// Author: Eliot Muir
// Date:   Thursday, November 18th, 2010 @ 02:43:52 PM
//---------------------------------------------------------------------------

#include <LUA/LUAheader.h>

int TRNcountOfLocals(lua_State* L, lua_Debug* pD);

#endif // end of defensive include
