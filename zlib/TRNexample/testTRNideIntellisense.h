#ifndef __TEST_T_R_NIDE_INTELLISENSE_H__
#define __TEST_T_R_NIDE_INTELLISENSE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testTRNideIntellisense
//
// Description:
//
// Tests for auto completion.
//
// Author: Eliot Muir 
// Date:   Tuesday 02 May 2023 - 10:14AM
// ---------------------------------------------------------------------------

#include <LUA/LUAutils.h>
#include <COL/COLhashmap.h>

class UNITapp;
class DBdatabaseFactory;

void testTRNideIntellisense(UNITapp* pApp);
void testRegister(lua_State* L, LUAmode Mode, DBdatabaseFactory* pFactory, COLhashmap<COLstring, COLstring>* pHelpMap);
void loadHelp(COLhashmap<COLstring, COLstring>* pHelpMap);

#endif // end of defensive include
