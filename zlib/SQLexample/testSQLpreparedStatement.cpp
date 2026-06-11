//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testSQLsqlite
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday, July 25th, 2005 @ 08:35:55 PM
//
//---------------------------------------------------------------------------
#include <SQLexample/SQLexamplePrecomp.h>
#pragma hdrstop
#include <SQLTE/sqlite3.h>

#include <FIL/FILutils.h>

#include <COL/COLdateTime.h>
#include <COL/COLlog.h>
#include <SQL/SQLsqlite.h>
#include <SQL/SQLtransaction.h>
#include <SQL/SQLpreparedStatement.h>
#include <SQL/SQLstatementIterator.h>
COL_LOG_MODULE;

static void prepareForTest()
{
   const char* pFileName = "prepare_test.db";
   if (FILfileExists(pFileName))
   {
      FILremoveWithThrow(pFileName);
   }

   SQLsqlite Database;
   Database.open("prepare_test.db");
   Database.executeSqlString("CREATE TABLE PREPARE_TEST(TestIndex INTEGER PRIMARY KEY, TestData TEXT);");
}

void testSQLpreparedStatementBasic()
{
   prepareForTest();
   SQLsqlite Database;
   Database.open("prepare_test.db");

   SQLpreparedStatement InsertStatement;
   InsertStatement.prepare(Database,"INSERT INTO PREPARE_TEST (TestIndex, TestData) VALUES(?,?);");

   const COLuint32 MaxInsert = 300;
   {
      SQLtransaction Transaction(Database);
      for (COLindex InsertIndex = 0; InsertIndex != MaxInsert; InsertIndex++)
      {
         InsertStatement.bindInteger64Value(1,InsertIndex);
         COLstring InsertTestVal;
         COLostream InsertStream(InsertTestVal);
         InsertStream << "SOMEVAL @ " << InsertIndex;
         InsertStatement.bindTextValue(2,InsertTestVal.c_str(),InsertTestVal.size());
         {
            SQLstatementIterator Iterator(InsertStatement);
            Iterator.stepNext();
         }
      }
      Transaction.commit();
   }
   
   SQLpreparedStatement SelectStatement;
   SelectStatement.prepare(Database,"SELECT * from PREPARE_TEST WHERE TestIndex BETWEEN ? AND ?;");
   SelectStatement.bindInteger64Value(1,0);
   SelectStatement.bindInteger64Value(2,MaxInsert);
   COLuint32 ResultCount = 0;
   for(SQLstatementIterator Iterator(SelectStatement); Iterator.stepNext();)
   {
      ResultCount++;
      const char* pString = "";
      Iterator.textValue(1,&pString);
      COL_VAR(pString);
   }
   TST_ASSERT(ResultCount == MaxInsert);
}

TSTtestable* testSQLpreparedStatementTests()
{
   TST_START_COLLECTION("testSQLpreparedStatement")
      TST_ADD_TEST_FUNCTION(testSQLpreparedStatementBasic);
   TST_END_FIXTURE
}
