#ifndef __LEDI_CACHE_H__
#define __LEDI_CACHE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LEDIcache
//
// Description:
//
// Function to get the EDIcollection* from the Lua registry. This creates and 
// caches it if not found.
//
// Author: Vismay Shah 
// Date:   Tuesday 13 June 2023 - 02:04PM
// ---------------------------------------------------------------------------
#include <LND/LNDcacheInfo.h>
class EDIcollection;
struct lua_State;

class LEDIcacheItem{
public:
   LEDIcacheItem();
   ~LEDIcacheItem();
   EDIcollection* m_pCollection = nullptr;
   LNDcacheInfo   m_CacheInfo;
};

EDIcollection* LEDIcacheGetEdiGrammar(lua_State* L, const COLstring& File, bool* pIsLegacy, COLstring* pConversionWarning);

#endif // end of defensive include
