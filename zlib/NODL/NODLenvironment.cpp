//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODLinitLuaEnvironment
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Tuesday, August 17th, 2010 @ 02:57:31 PM
//---------------------------------------------------------------------------
#include "NODLenvironment.h"

#include "NODLutils.h"
#include <new> // For placement new

#include <LUA/LUAerror.h>
#include <LUA/LUAutils.h>
#include <DB/DBdatabaseFactory.h>
#include <DB/DBdatabaseOdbc.h>

#include <COL/COLlog.h>
#include <COL/COLhashmap.h>
#include <COL/COLdateTime.h>

#ifdef _WIN32
#include <ObjBase.h>
#endif
COL_LOG_MODULE;

static const char
   NODL_DATABASE_PTR_CACHE_METATABLE[] = "DatabaseCacheMetatable";

const COLstring NODL_TEST_ENVIRONMENT_REGISTRY_NAME = "TestEnvironment";
const COLstring NODL_DATABASE_FACTORY = "DatabaseFactory";
const COLstring NODL_DATABASE_PTR_CACHE = "DatabaseCache";
const COLstring NODL_DB_CONNECTION_COUNT = "DbConnectionCount";

struct NODLdatabaseCacheItem{
   DBdatabasePtr  pDatabase;
   COLtimeStamp   LastAccessTimeStamp;

   void touch(){
      COLcurrentTimeStamp (&LastAccessTimeStamp);
   }

   bool closeOnInactivity (lua_State* L) {
      const int MAX_CONNECTION_IDLE_TIME = 60; // In seconds
      COLtimeStamp CurrentTime;
      COLcurrentTimeStamp (&CurrentTime);
      if (CurrentTime.tv_sec - LastAccessTimeStamp.tv_sec > MAX_CONNECTION_IDLE_TIME){
         pDatabase->disconnect();
         NODLdbDecrementConnectionCount(L);         
         return true;
      }
      return false;
   }

   NODLdatabaseCacheItem(DBdatabasePtr  pDB = DBdatabasePtr()) : pDatabase (pDB), LastAccessTimeStamp() { touch(); }
};

typedef COLhashmap <COLstring, NODLdatabaseCacheItem> NODLdatabaseCache;

static NODLdatabaseCache* NODLgetDatabaseCache(lua_State* L){
   COL_FUNCTION(NODLgetDatabaseCache);
   const int StackTop = lua_gettop (L); 
   LUApushString(L, NODL_DATABASE_PTR_CACHE);
   lua_gettable(L, LUA_REGISTRYINDEX);
   COLASSERT(!lua_isnil(L, -1));
   void* UserData = LUAcheckUdata(L, -1, NODL_DATABASE_PTR_CACHE_METATABLE, "`database ptr cache' expected");
   lua_pop(L, 1); // Remove the table from the stack
   COLASSERT(StackTop == lua_gettop (L));  // in NODLgetDatabaseCache 
   return (NODLdatabaseCache*)UserData;
}

void NODLcreateDatabaseCache(lua_State* L){
   COL_FUNCTION(NODLcreateDatabaseCache);
   const int StackTop = lua_gettop (L);
   LUApushString(L, NODL_DATABASE_PTR_CACHE);
   lua_gettable(L, LUA_REGISTRYINDEX);
   COLASSERT(lua_isnil(L, -1)); // DB cache should not be in the registry
   lua_pop (L, 1); // Swallow the nil table pointer
   LUApushString(L, NODL_DATABASE_PTR_CACHE);
   LUApushObject<NODLdatabaseCache>(L, NODL_DATABASE_PTR_CACHE_METATABLE);
   lua_settable(L, LUA_REGISTRYINDEX);
   COLASSERT(StackTop == lua_gettop (L)) //  in NODLcreateDatabaseCache 
}

// void NODLsetPreviewListener(lua_State* L, NODLpreviewListener* pListener){
//    LUApushString(L, NODL_PREVIEW_LISTENER);
//    lua_pushlightuserdata(L, pListener);
//    lua_settable(L, LUA_REGISTRYINDEX);
// }

// void NODLunsetPreviewListener(lua_State* L){
//    LUApushString(L, NODL_PREVIEW_LISTENER);
//    lua_pushnil(L);
//    lua_settable(L, LUA_REGISTRYINDEX);
// }

static void NODLdbAdjustConnectionCount(lua_State* L, int CountChange) {
   LUApushString(L, NODL_DB_CONNECTION_COUNT);   
   lua_gettable(L, LUA_REGISTRYINDEX);   

   int CurConnectionCount = (int) lua_tointeger(L, -1) + CountChange;    
   lua_pop(L, 1);

   LUApushString(L, NODL_DB_CONNECTION_COUNT);   
   lua_pushinteger(L, CurConnectionCount);   
   lua_settable(L, LUA_REGISTRYINDEX);   
}

void NODLdbIncrementConnectionCount(lua_State* L) {
   NODLdbAdjustConnectionCount(L, 1);
}

void NODLdbDecrementConnectionCount(lua_State* L) {
   NODLdbAdjustConnectionCount(L, -1);
}

COLstring NODLdbCacheString(const COLstring& Api, const COLstring& Db, const COLstring& User, 
                            const COLstring& Password, bool UseUnicode, const COLstring& EncryptionKey) {
   COL_FUNCTION(NODLdbCacheString);
      COLstring DbCacheString; {
         COLostream Stream(DbCacheString); Stream
            << Api  << '\t' << Db       << '\t'
            << User << '\t' << Password << '\t'
            << (UseUnicode ? "Unicode" : "Raw") << '\t' << EncryptionKey;
      }
      return DbCacheString;
}

DBdatabasePtr NODLdbGetDatabase(lua_State* L,
                                const COLstring& Api,
                                const COLstring& Db,
                                const COLstring& User,
                                const COLstring& Password,
                                bool UseUnicode,
                                int TimeoutSeconds,
                                bool ConnectionCaching,
                                const COLstring& EncryptionKey)
{
   COL_FUNCTION (NODLdbGetDatabase);
   
   const int StackTop = lua_gettop (L);
   LUApushString(L, NODL_DATABASE_FACTORY);
   lua_gettable(L, LUA_REGISTRYINDEX);
   COL_TRC("Getting database factory from state");
   DBdatabaseFactory* pDatabaseFactory = (DBdatabaseFactory*)lua_touserdata(L, -1);
   COL_VAR(pDatabaseFactory);
   lua_pop(L, 1);
   DBdatabasePtr pDatabase;
   NODLdatabaseCache* pDbCache;
   COLstring DbCacheString;
   COLhashmapPlace Place;

   COL_TRC("Checking for connection caching");
   if (ConnectionCaching) {
      pDbCache = NODLgetDatabaseCache(L);
      COLASSERT(pDbCache);   
      
      DbCacheString = NODLdbCacheString(Api, Db, User, Password, UseUnicode, EncryptionKey);
      COL_VAR(DbCacheString);

      Place = pDbCache->find(DbCacheString);
      if (Place != 0){
         pDatabase = ((*pDbCache)[Place].pDatabase);
      }
   }

   //i.e. didn't load a cached connection
   if (!pDatabase.get()) {
      COL_TRC("No connection caching or didn't load cached connection. Getting new database pointer");
      pDatabase = pDatabaseFactory->getDatabase(Api);
      
      if (UseUnicode) {
         pDatabase->setParameter(DB_PARAM_USE_UNICODE, "yes");
      }

      if (ConnectionCaching) {
         Place = pDbCache->add(DbCacheString, NODLdatabaseCacheItem(pDatabase));
      }
   }

   if (TimeoutSeconds >= 0) {
      if (dynamic_cast <DBdatabaseOdbc*> (pDatabase.get()) == NULL) {
         LUAraiseLuaError (L, "Parameter 'timeout' is supported only for ODBC based connections.");
      }

      pDatabase->setTimeout (TimeoutSeconds);
   }
   
   if (ConnectionCaching) {
      try {      
         ((*pDbCache)[DbCacheString]).touch();
      } 
      catch (COLerror &Err){
         pDbCache->remove(Place);
         throw Err;
      }
   }

   //support binary if the driver allows it.
   pDatabase->setSupportBinaryType(true);
   COLASSERT (StackTop == lua_gettop (L)); 
   return pDatabase;
}

DBdatabasePtr NODLdbRemoveFromCache(lua_State* L, const COLstring& Api, const COLstring& Db, const COLstring& User, const COLstring& Password, bool UseUnicode, const COLstring& EncryptionKey) {
   COL_FUNCTION (NODLdbRemoveFromCache);
   const int StackTop = lua_gettop (L);
   NODLdatabaseCache *pDbCache = NODLgetDatabaseCache (L);
   COLASSERT(pDbCache);
   const COLstring DbCacheString = NODLdbCacheString(Api, Db, User, Password, UseUnicode, EncryptionKey);
   COLhashmapPlace Place = pDbCache->find(DbCacheString) ;
   DBdatabasePtr pDatabase;

   if (Place != 0){
     pDatabase = (*pDbCache)[Place].pDatabase;
     pDbCache->remove(Place);
   }
   COLASSERT (StackTop == lua_gettop (L)); 

   return pDatabase;
}

void NODLdbInactiveConnectionCleanup(lua_State* L){
   COL_FUNCTION(NODLdbInactiveConnectionCleanup);
   NODLdatabaseCache *DbCache = NODLgetDatabaseCache (L);
   COLASSERT (DbCache);

   COLarray <COLhashmapPlace> MembersToRemove; 

   for (COLhashmapPlace Place = DbCache->first(); Place; Place = DbCache->next(Place)){
      NODLdatabaseCacheItem &CacheItem = (*DbCache)[Place];
      if (CacheItem.closeOnInactivity(L)) {
         MembersToRemove.push_back(Place);
      }
   }

   for (int i = 0; i < MembersToRemove.size (); ++i) {
      DbCache->remove(MembersToRemove[i]);
   }
}

int NODLgetDbConnectionCount(lua_State* L){
   int CountOfActiveConnections = 0;
   if (L) {
      LUApushString(L, NODL_DB_CONNECTION_COUNT);      
      lua_gettable(L, LUA_REGISTRYINDEX);      
      if (lua_isnumber(L, -1)) {
         CountOfActiveConnections = (int) lua_tointeger(L, -1);
      }
      lua_pop(L, 1);      
   }

   return CountOfActiveConnections;
}
