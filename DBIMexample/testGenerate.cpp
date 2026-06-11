//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testGenerate
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Mon 27 Nov 2023 5:45:43 pm
//---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>

#include <DB/DBgrammar.h>
#include <DB/DBgrammarLoad.h>
#include <DB/DBdatabasePtr.h>
#include <DBIM/DBIMcreateTable.h>
#include <DBIM/DBIMgenerateSql.h>
#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testSimple(){
   COL_FUNCTION(testSimple);
   DBcollection Grammar;
   COLstring Sql;
   COL_TRC("Read vdb into DBcollection");
   COLstring VdbData;
   FILreadFile("schema.vdb", &VdbData);
   COLstring Error;
   DBload(VdbData, &Grammar, &Error);
   COLmap<COLstring, bool> ExistingTables;
   COLarray<DBIMcreateTable> TablesToCreate;
   DBIMmakeCreateStatements(TablesToCreate, Grammar);
   Sql = DBIMgenerateCreateStatements(TablesToCreate, ExistingTables, DB_SQLITE);
   COL_VAR(Sql);
   Sql.stripAll('\r');
   Sql.stripAll('\n');
   Sql.stripAll('\t');
   COLstring ExpectedResult = 
   "CREATE TABLE \"Patient\"("
	   "\"id\" TEXT,"
	   "\"firstname\" TEXT,"
	   "\"lastname\" TEXT"
   ");";
   UNIT_ASSERT_EQUALS(ExpectedResult, Sql);
}

void testGenerate(UNITapp* pApp){
   pApp->add("generate/simple", &testSimple);
}
