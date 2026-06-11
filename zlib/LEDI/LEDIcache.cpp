// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LEDIcache
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Tuesday 13 June 2023 - 02:04PM
// ---------------------------------------------------------------------------
#include <LEDI/LEDIcache.h>

#include <EDI/EDIgrammar.h>
#include <EDI/EDIload.h>
#include <FIL/FILutils.h>

#include <LND/LNDcache.h>
#include <LUA/LUAerror.h>
#include <LUA/LUAutils.h>
#include <LUAL/LUALutils.h>

#include <COL/COLlog.h>
// #include <COL/COLsafeCount.h> // keep commented out for release
COL_LOG_MODULE;

LEDIcacheItem::LEDIcacheItem(){
   COL_METHOD(LEDIcacheItem::LEDIcacheItem);
   // COLincCount();
}

LEDIcacheItem::~LEDIcacheItem(){
   COL_METHOD(LEDIcacheItem::~LEDIcacheItem);
   // COLdecCount();
   if (m_pCollection) { delete m_pCollection; }
   m_pCollection = NULL;
}

static bool LEDIloadEdiCollection(const COLstring& File, EDIcollection* pCollection, COLstring *pError, COLstring *pConversionWarning, bool* pLegacyWarning){
   COL_FUNCTION(LEDIloadEdiCollection);
   COLstring Grammar;
   FILreadFile(File, &Grammar);
   return EDIsmartLoad(Grammar, pCollection, pError, pConversionWarning, pLegacyWarning); // EDIsmartLoad will clear pGrammar and populate it again.
}

static EDIcollection* LEDIcreateEdiGrammar(lua_State* L, const COLstring& File, LEDIcacheItem* pItem){
   COL_FUNCTION(LEDIcreateEdiGrammar);
   EDIcollection* pCollection = new EDIcollection();
   COLstring Error;
   COL_VAR(File);
   if (!LEDIloadEdiCollection(File, pCollection, &Error, &pItem->m_CacheInfo.m_ConversionWarning, &pItem->m_CacheInfo.m_IsLegacy)){
      COL_VAR(Error);
      delete pCollection;
      LUAraiseLuaError(L, Error); // This will exit out of this function and back to lua
   }
   return pCollection;
}

static void LEDIpopulateEdiItem(LEDIcacheItem* pNewItem, EDIcollection* pCollection, const COLstring& File, bool LegacyWarning, const time_t& LastWrite){
   COL_FUNCTION(LEDIpopulateEdiItem);
   pNewItem->m_pCollection  = pCollection;
   LNDpopulateCacheInfo(&pNewItem->m_CacheInfo, File, LegacyWarning, pNewItem->m_CacheInfo.m_ConversionWarning, LastWrite);
}

LEDIcacheItem* LEDIcacheItemGet(lua_State* L, const COLstring& Key){
   COL_FUNCTION(LEDIcacheItemGet);
   return (LEDIcacheItem*)LUAgetPointerField(L, Key.c_str());
}


EDIcollection* LEDIcacheGetEdiGrammar(lua_State* L, const COLstring& File, bool* pIsLegacy, COLstring* pConversionWarning){
   COL_FUNCTION(LEDIcacheGetEdiGrammar);
   COLstring Key = LNDvmdKey(File);
   time_t LastWrite;
   if(LNDkeyExists(L, Key)) {
      COL_TRC(Key << "found in cache, checking to see if it needs a reload");
      LEDIcacheItem* pItem = LEDIcacheItemGet(L, Key);
      COL_VAR3(pItem, &pItem->m_CacheInfo, pItem->m_CacheInfo.m_LastAccess);
      COL_VAR(pItem->m_pCollection);
      if (LNDisItemOutdated(File, &LastWrite, pItem->m_CacheInfo.m_LastAccess)){ 
         COL_TRC("Reload required - reloading and storing updated version in cache");
         EDIcollection* pNewCollection = LEDIcreateEdiGrammar(L, File, pItem); // Try creation first, in case it fails. If it fails it will return out to lua without doing anything else
         delete pItem->m_pCollection;
         *pIsLegacy = pItem->m_CacheInfo.m_IsLegacy;
         LEDIpopulateEdiItem(pItem, pNewCollection, File, *pIsLegacy, LastWrite);
         *pConversionWarning = pItem->m_CacheInfo.m_ConversionWarning;
         COL_VAR2(&pItem->m_pCollection->Nodes,  pItem->m_pCollection->Nodes.size());
         return pItem->m_pCollection;
      } else {
         COL_TRC("No need to reload, safe to re-use cached grammar");
         *pIsLegacy = pItem->m_CacheInfo.m_IsLegacy;
         *pConversionWarning = pItem->m_CacheInfo.m_ConversionWarning;
         COL_VAR2(&pItem->m_pCollection->Nodes,  pItem->m_pCollection->Nodes.size());
         return pItem->m_pCollection;
      }
   } else {
      COL_TRC(Key << " not yet in cache, adding now");
      LNDcacheFileExists(L, File);
      LNDgetLastWriteTime(File, &LastWrite);
      LEDIcacheItem* pNewItem = LUApushObject<LEDIcacheItem>(L, "LEDIcollectionCache");
      EDIcollection* pCollection = LEDIcreateEdiGrammar(L, File, pNewItem);
      *pIsLegacy = pNewItem->m_CacheInfo.m_IsLegacy;
      LEDIpopulateEdiItem(pNewItem, pCollection, File, *pIsLegacy, LastWrite);
      *pConversionWarning = pNewItem->m_CacheInfo.m_ConversionWarning;
      COL_TRC("Adding " << Key << " to cache");
      lua_setfield(L, LUA_REGISTRYINDEX, Key.c_str());
      return pCollection;
   }
}