#ifndef __NODL_MERGE_H__
#define __NODL_MERGE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODLmerge
//
// Description:
//
// Database merge tables functions
//
// Author: Vismay Shah 
// Date:   Monday 05 December 2022
// ---------------------------------------------------------------------------
struct lua_State;

class DBdatabasePtr;
class NTABcollection;

// NOTE: these used to take a NODLlog* for interacting with the logging system. We'll need to restore this behaviour
int NODLdoMergeTablesNotBulk      (lua_State* L, DBdatabasePtr pDatabase, const NTABcollection* pRootNode);
int NODLdoMergeTablesBulk         (lua_State* L, DBdatabasePtr pDatabase, const NTABcollection* pRootNode);
int NODLdoMergeTablesBulkSQLServer(lua_State* L, DBdatabasePtr pDatabase, const NTABcollection* pRootNode);

#endif // end of defensive include
