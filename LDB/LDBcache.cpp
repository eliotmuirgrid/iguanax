// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LDBcache
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Monday 12 June 2023 - 03:55PM
// ---------------------------------------------------------------------------
#include <LDB/LDBcache.h>

#include <DB/DBgrammarLoad.h>
#include <FIL/FILutils.h>
#include <LUA/LUAerror.h>
#include <LUA/LUAtable.h>
#include <LUA/LUAutils.h>
#include <LND/LNDcache.h>

#include <NTAB/NTABschema.h>

#include <COL/COLlog.h>
// #include <COL/COLsafeCount.h> // keep commented out for release
COL_LOG_MODULE;

LDBcacheItem::LDBcacheItem(){
   COL_METHOD(LDBcacheItem::LDBcacheItem);
   // COLincCount();
}

LDBcacheItem::~LDBcacheItem(){
   COL_METHOD(LDBcacheItem::~LDBcacheItem);
   // COLdecCount();
   if (m_pSchema) {
      delete m_pSchema;
   }
   m_pSchema = NULL;
}

static bool LDBloadDbCollection(const COLstring& File, DBcollection* pDbGrammar, COLstring *pError, bool* pLegacyWarning){
   COL_FUNCTION(LDBloadDbCollection);
   COLstring Grammar; 
   FILreadFile(File, &Grammar);
   return DBsmartLoad(Grammar, pDbGrammar, pError, pLegacyWarning); // DBsmartLoad will clear pDbGrammar and populate it again.
}

static NTABschema* LDBcreateDbSchema(lua_State* L, const COLstring& File, LDBcacheItem* pItem){
   COL_FUNCTION(LDBcreateDbSchema);
   DBcollection DatabaseGrammar;
   COLstring Error;
   if(!LDBloadDbCollection(File, &DatabaseGrammar, &Error, &pItem->m_CacheInfo.m_IsLegacy)) { 
      COL_VAR(Error);
      LUAraiseLuaError(L, Error);
   }
   NTABschema* pDbSchema = new NTABschema(&DatabaseGrammar);
   return pDbSchema;
}

static void LDBpopulateSchemaItem(LDBcacheItem* pNewItem, NTABschema* pDbSchema, const COLstring& File, bool LegacyWarning, const time_t& LastWrite){
   COL_FUNCTION(LDBpopulateSchemaItem);
   pNewItem->m_pSchema = pDbSchema;
   LNDpopulateCacheInfo(&pNewItem->m_CacheInfo, File, LegacyWarning, COLstring(), LastWrite);
}

LDBcacheItem* LDBcacheItemGet(lua_State* L, const COLstring& Key){
   COL_FUNCTION(LDBcacheItemGet);
   return (LDBcacheItem*)LUAgetPointerField(L, Key.c_str());
}

NTABschema* LDBgetDbSchema(lua_State* L, const COLstring& File, bool* pLegacyWarning){
   COL_FUNCTION(LDBgetDbSchema);
   COLstring Key = LNDvdbKey(File);
   time_t LastWrite;
   if(LNDkeyExists(L, Key)) {
      COL_TRC(Key << "found in cache, checking to see if it needs a reload");
      LDBcacheItem* pItem = LDBcacheItemGet(L, Key);
      if (LNDisItemOutdated(File, &LastWrite, pItem->m_CacheInfo.m_LastAccess)){ 
         COL_TRC("Reload required - reloading and storing updated version in cache");
         NTABschema* pDatabaseSchema = LDBcreateDbSchema(L, File, pItem);// Try creation first, in case it fails. If it fails it will return out to lua without doing anything else
         delete pItem->m_pSchema;
         LDBpopulateSchemaItem(pItem, pDatabaseSchema, File, *pLegacyWarning, LastWrite);
         return pItem->m_pSchema;
      } else {
         COL_TRC("No need to reload, safe to re-use cached grammar");
         return pItem->m_pSchema;
      }
   } else {
      COL_TRC(Key << " not yet in cache");
      LNDcacheFileExists(L, File);
      LNDgetLastWriteTime(File, &LastWrite);
      LDBcacheItem* pNewItem = LUApushObject<LDBcacheItem>(L, "LDBschemaCache");
      NTABschema* pDatabaseSchema = LDBcreateDbSchema(L, File, pNewItem);
      LDBpopulateSchemaItem(pNewItem, pDatabaseSchema, File, *pLegacyWarning, LastWrite);
      COL_TRC("Adding " << Key << " to cache");
      lua_setfield(L, LUA_REGISTRYINDEX, Key.c_str());
      return pDatabaseSchema;
   }
}

