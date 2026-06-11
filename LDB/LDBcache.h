#ifndef __LDB_CACHE_H__
#define __LDB_CACHE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LDBcache
//
// Description:
//
// Function to get the NTABschema* from the Lua registry. This creates and 
// caches it if not found.
//
// Author: Vismay Shah 
// Date:   Monday 12 June 2023 - 03:55PM
// ---------------------------------------------------------------------------
#include <LND/LNDcacheInfo.h>
class NTABschema;
struct lua_State;

class LDBcacheItem{
public:
   LDBcacheItem();
   ~LDBcacheItem();
   NTABschema*  m_pSchema = nullptr;
   LNDcacheInfo m_CacheInfo;
};

NTABschema* LDBgetDbSchema(lua_State* L, const COLstring& File, bool* pLegacyWarning);

#endif // end of defensive include
