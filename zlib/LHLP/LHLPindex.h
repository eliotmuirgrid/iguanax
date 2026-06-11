#ifndef __HELP_INDEX_H__
#define __HELP_INDEX_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LHLPindex
//
// Description:
//
// APIs for the Help system
//
// Author: Vismay Shah
//---------------------------------------------------------------------------
#include <COL/COLhashmap.h>

struct lua_State;

void LHLPregisterCustomHelp(lua_State* L, COLhashmap<COLstring, COLstring>* pHelpMap, const COLstring& ProjectPath);

void LHLPinit(COLhashmap<COLstring, COLstring>* pMap, lua_State* L);

void LHLPcreateHelpTable(lua_State* L);

void LHLPluaHelpIndex(const COLhashmap<COLstring, COLstring>& Map, lua_State* L);

#endif // end of defensive include