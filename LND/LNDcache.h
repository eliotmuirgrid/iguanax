#ifndef __LND_CACHE_H__
#define __LND_CACHE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LNDcache
//
// Description:
// 
// Helper functions for interacting with a cached NTABschema or EDIcollection
// stored in the lua registry. Used by apis like hl7.parse and db.tables.
//
// Author: Vismay Shah
// Date: Tues Nov 22 2022
//---------------------------------------------------------------------------
#include <time.h>
struct lua_State;
class COLstring;

void      LNDcacheFileExists(lua_State* L, const COLstring& File);

bool      LNDkeyExists(lua_State* L, const COLstring& Key);

void      LNDgetLastWriteTime(const COLstring& File, time_t* pTime);
bool      LNDisItemOutdated  (const COLstring& File, time_t* pLastWrite, const time_t& CacheLastAccess);

COLstring LNDvdbKey(const COLstring& File);
COLstring LNDvmdKey(const COLstring& File);

#endif // end of defensive include
