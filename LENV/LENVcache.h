#ifndef __LENV_CACHE_H__
#define __LENV_CACHE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LENVcache
//
// Description:
//
// TODO
//
// Author: Vismay Shah 
// Date:   Thursday 30 January 2025 - 02:52PM
// ---------------------------------------------------------------------------
#include <COL/COLvar.h>
struct lua_State;

COLvar LENVgetEnvironment();
void LENVcache(lua_State* L, const COLvar& Environment);
void LENVgetCache(lua_State* L, COLvar* pCachedEnvironmentVars);
#endif // end of defensive include
