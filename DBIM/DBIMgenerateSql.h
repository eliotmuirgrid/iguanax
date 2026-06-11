#ifndef __DBIM_GENERATE_SQL_H__
#define __DBIM_GENERATE_SQL_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBIMgenerateSql
//
// Description:
//
// Generates the create statements to create all tables in a vdb file.
//
// Author: Vismay Shah 
// Date:   Tuesday 07 March 2023 - 10:07AM
// ---------------------------------------------------------------------------
#include <COL/COLarray.h>

class COLwebRequest;
class DBdatabaseFactory;
class DBcollection;
class DBIMcreateTable;
class SCKloop;

void DBIMgenerateSql(const COLwebRequest& Request, SCKloop* pLoop, DBdatabaseFactory* pDatabaseFactory);

// Exposed for unit testing
void DBIMmakeCreateStatements(COLarray<DBIMcreateTable>& TablesToCreate, DBcollection& Grammar);

#endif // end of defensive include
