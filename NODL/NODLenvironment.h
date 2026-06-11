#ifndef __NODL_ENVIRONMENT_H__
#define __NODL_ENVIRONMENT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODLinitLuaEnvironment
//
// Description:
//
// NODLinitLuaEnvironment class.
//
// Author: Kevin Senn
// Date:   Tuesday, August 17th, 2010 @ 02:57:31 PM
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <LUA/LUAheader.h>

// #include "NODLpreviewListener.h"

class DBdatabaseFactory;
class DBtransaction;
class NODplace;
class NODLmessageId;

#include <DB/DBdatabasePtr.h>

extern const COLstring NODL_DATABASE_FACTORY;
extern const COLstring NODL_DB_CONNECTION_COUNT;

// void NODLsetPreviewListener(lua_State* L, NODLpreviewListener* pListener); Vismay  : what are these?
// void NODLunsetPreviewListener(lua_State* L);

//Adjust counter of active db connections
void NODLdbIncrementConnectionCount(lua_State* L);
void NODLdbDecrementConnectionCount(lua_State* L);

//Returns a database pointer based on the provided parameters and implements connection caching.
//If ConnectionCaching is true, it will look for a cached database pointer.
DBdatabasePtr NODLdbGetDatabase(
   lua_State* L,
   const COLstring& DatabaseApi,
   const COLstring& DataSource,
   const COLstring& UserName,
   const COLstring& Password,
   bool UseUnicode,
   int TimeoutSeconds,
   bool ConnectionCaching,
   const COLstring& EncryptionKey
);

//Removes cached database pointer for provided parameters and returns it (or an empty one if not found).
DBdatabasePtr NODLdbRemoveFromCache(
   lua_State* L,
   const COLstring& DatabaseApi,
   const COLstring& DataSource,
   const COLstring& UserName,
   const COLstring& Password,
   bool UseUnicode,
   const COLstring& EncryptionKey
);

void NODLdbInactiveConnectionCleanup (lua_State* L);
int NODLgetDbConnectionCount(lua_State* L);

void NODLcreateDatabaseCache(lua_State* L);

#endif // end of defensive include
