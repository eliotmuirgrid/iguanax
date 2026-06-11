//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
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

#include <COL/COLmutex.h>
#include <COL/COLthread.h>
#include <COL/COLthreadPool.h>
#include <COL/COLmath.h>
#include <COL/COLdateTime.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <SQL/SQLsqlite.h>
#include <SQL/SQLtransaction.h>
#include <SQL/SQLpreparedStatement.h>
#include <SQL/SQLstatementIterator.h>
#include <SQL/SQLbusyHandler.h>

#include <COL/COLutils.h>

#include "testSQLutils.h"

static const COLstring TEST_UTF8_DATABASE_FILENAME = "字字字prepare_test.db";
static const COLstring TEST_SD_GUID = "會意字-To-ERnAHGI1Nib74j";
//static const COLstring TEST_UTF8_DATABASE_FILENAME = "\xF0\x9F\x8D\x95\xF0\x9F\x8D\x94prepare_test.db";

/*static void testSampleDataSetup() {
   COL_FUNCTION(testSampleDataSetup);
   const COLstring DbName = "data/" + TEST_SD_GUID + ".db";

   SQLsqlite Database;

   NLOGopenDatabase(Database, TEST_SD_GUID);

   NLOGcreateTables(Database);

   SQLpreparedStatement TableInspection;
   TableInspection.prepare(Database, "SELECT tbl_name FROM sqlite_master");
   SQLstatementIterator SelectIter(TableInspection);
   SQLstatementIterator::IterateResult Result;
   COLvar TablesSeen;
   while (SelectIter.stepNext()) {
      TablesSeen[SelectIter.textValueStr(0)] = true;
   }
   COL_VAR(TablesSeen.json(true, COLvar::StrEncJs));
   TST_ASSERT(TablesSeen.exists("Log"));
   TST_ASSERT(TablesSeen.exists("LogCache"));
   TST_ASSERT(TablesSeen.exists("LogCount"));
   TST_ASSERT(TablesSeen.exists("TestResults"));
   TST_ASSERT(TablesSeen.exists("TestStatus"));

   NLOGupgradeDatabase(Database);

   SQLpreparedStatement TableInspection2;
   TableInspection2.prepare(Database, "SELECT tbl_name FROM sqlite_master");
   SQLstatementIterator SelectIter2(TableInspection2);
   SQLstatementIterator::IterateResult Result2;
   TablesSeen.reset();
   while (SelectIter2.stepNext()) {
      TablesSeen[SelectIter2.textValueStr(0)] = true;
   }
   COL_VAR(TablesSeen.json(true, COLvar::StrEncJs));
   TST_ASSERT(TablesSeen.exists("Log"));
   TST_ASSERT(TablesSeen.exists("LogCache"));
   TST_ASSERT(TablesSeen.exists("LogCount"));
   TST_ASSERT(TablesSeen.exists("TestResults"));
   TST_ASSERT(TablesSeen.exists("TestStatus"));
}*/

static void resetFile(){
   if (FILfileExists(TEST_UTF8_DATABASE_FILENAME)) {
      FILremoveWithThrow(TEST_UTF8_DATABASE_FILENAME);
   }

   SQLsqlite Database;
   Database.open(TEST_UTF8_DATABASE_FILENAME);
   Database.executeSqlString("CREATE TABLE PREPARE_TEST(TestIndex INTEGER PRIMARY KEY AUTOINCREMENT, TestData TEXT);");
   Database.executeSqlString("CREATE TABLE PREPARE_TEST2(TestIndex INTEGER PRIMARY KEY AUTOINCREMENT, TestData TEXT);");
}

static void prepareForTest()
{
   resetFile();
   SQLsqlite Database;
   Database.open(TEST_UTF8_DATABASE_FILENAME);

   const COLuint32 MaxInsert = 300;
   {
      SQLtransaction Transaction(Database);
      SQLpreparedStatement InsertStatement;
      InsertStatement.prepare(Database,"INSERT INTO PREPARE_TEST (TestIndex, TestData) VALUES(?,?);");
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
}

static void testSQLstatementIteratorBusy()
{
   prepareForTest();
   SQLsqlite Database;
   Database.open(TEST_UTF8_DATABASE_FILENAME);
   //one to actually read
   SQLpreparedStatement SelectStatement;
   SelectStatement.prepare(Database,"SELECT * from PREPARE_TEST");
   SQLstatementIterator SelectIter(SelectStatement);

   //open a write transaction to hold an exclusive lock, on a separate database connection
   SQLsqlite Database2;
   Database2.open(TEST_UTF8_DATABASE_FILENAME);
   SQLexclusiveTransaction TransactionEx(Database2);

   {
      testSQLiteratorBusyHandler BusyHandler;
      BusyHandler.reset(10);
      BusyHandler.setSqliteBusyInterval(10);
      SQLstatementIterator::IterateResult Result;
      Result = SelectIter.stepNextBusy(BusyHandler);

      TST_ASSERT(Result == SQLstatementIterator::Interrupted)
      TST_ASSERT(BusyHandler.CountOfCalled == 10);
 
      //release the lock and try again
      TransactionEx.commit();

      BusyHandler.reset(10);
      Result = SelectIter.stepNextBusy(BusyHandler);
      TST_ASSERT(Result == SQLstatementIterator::HaveRows)
      TST_ASSERT(BusyHandler.CountOfCalled == 0)

      //continue until the end
      while (Result == SQLstatementIterator::HaveRows)
      {
         BusyHandler.reset(10);
         Result = SelectIter.stepNextBusy(BusyHandler);
         TST_ASSERT(BusyHandler.CountOfCalled == 0)
      }
      TST_ASSERT(Result == SQLstatementIterator::NoRows)
   }
}

class SQLTwriterThread{
public:
   SQLTwriterThread() : TransactionType("BEGIN"), CurrentId(0), IsStopRequested(false)
   {
      Database.open(TEST_UTF8_DATABASE_FILENAME);
   }
   ~SQLTwriterThread(){
      stop();
   }

   void run(){
      try{
         COLuint32 MaxRows = 50;
         while(!isStopRequested() && MaxRows--)
         {
            COL_TRC("About to begin transaction");
            SQLtransaction Transaction(Database,TransactionType.c_str());

            //insert or update?
            COLint32 TestIndex = CurrentId++;

            bool DoUpdate = false;
            {
               SQLpreparedStatement Select;
               Select.prepare(Database, "SELECT TestIndex FROM Prepare_Test WHERE TestIndex=?");
               Select.bindInteger64Value(1,TestIndex);
               SQLstatementIterator SelectIter(Select);
               DoUpdate = SelectIter.stepNext();
            }
            COL_VAR(DoUpdate);
            SQLpreparedStatement InsertStatement;
            SQLpreparedStatement UpdateStatement;

            SQLpreparedStatement* pStatement = 0;
            if (DoUpdate){
               pStatement = &UpdateStatement;
               UpdateStatement.prepare(Database,"UPDATE Prepare_Test SET TestData=?1 WHERE TestIndex=?2;");
            }
            else{
               pStatement = &InsertStatement;
               InsertStatement.prepare(Database,"INSERT INTO Prepare_Test (TestData,TestIndex) VALUES(?,?);");
            }


            COLstring InsertTestVal;
            COLostream InsertStream(InsertTestVal);
            InsertStream << "SOMEVAL @ " << TestIndex << " thread_id="<< COLthreadGetCurrentThreadID();
            pStatement->bindTextValue(1,InsertTestVal.c_str(),InsertTestVal.size());
            pStatement->bindInteger64Value(2, TestIndex);

            {
               SQLstatementIterator Iterator(*pStatement);
               Iterator.stepNext();
            }

            
            {
               SQLpreparedStatement Insert2;
               Insert2.prepare(Database,"INSERT INTO Prepare_Test2(TestData) VALUES (?)");
               pStatement->bindTextValue(1,InsertTestVal.c_str(),InsertTestVal.size());
               SQLstatementIterator Iter(Insert2);
               Iter.stepNext();
            }

            Transaction.commit();
            COL_TRC("Transaction closed");
         }
      }
      
      catch(const COLerror& Error){
         ThisErr = Error;
      }
      COL_TRC("Run complete");
   }

   bool isStopRequested(){
      return IsStopRequested;
   }

   void stop(){
      IsStopRequested = true;
   }


   bool IsStopRequested;
   SQLsqlite Database;
   COLstring TransactionType;
   COLerror ThisErr;
   COLint32 CurrentId;
};


static void testTransactionMultipleWriter(const COLstring& Type, bool ExpectError)
{
   SQLTwriterThread W1;
   W1.TransactionType = Type;
   SQLTwriterThread W2;
   W2.TransactionType = Type;
   
   
   { 
      COLthreadPool Pool(2,2);
      Pool.start();
      Pool.scheduleTask(COLnewClosure0(&W2, &SQLTwriterThread::run));
      Pool.scheduleTask(COLnewClosure0(&W1, &SQLTwriterThread::run));

      COLuint32 MaxCount = 10;
      COLsleep(50);

      W1.stop();
      W2.stop();
   }

   COLstring Err1(W1.ThisErr.description());
   COLstring Err2(W2.ThisErr.description());


   COL_VAR(Err1);
   COL_VAR(Err2);

   if (!ExpectError){
      TST_ASSERT(Err1.is_null() && Err2.is_null());
   }
   else{
      TST_ASSERT(Err1.find("database is locked") != npos ||
                 Err2.find("database is locked") != npos);
   }
}


static void testMultipleWriterThreads()
{
   //Behavior is quite dependent on the transaction type
   //If a regular BEGIN is used, this test should result in a db locked error
   //since we'd get it in the middle of a transaction.

   // The default SQLite transaction is deferred, meaning the lock is only acquired
   // write operation is attempted. The first of the following tests tries to make
   // a race condition go bad, but it doesn't reliably cause the expected error.
   // This is new behaviour since we updated SQLite, and likely indicates
   // new efficiencies courtesy of D. Richard Hipp.
   // Sometimes the threads collide, sometimes not! Commenting it out for the moment.

   // testTransactionMultipleWriter("BEGIN",true);
   testTransactionMultipleWriter("BEGIN IMMEDIATE",false);
   testTransactionMultipleWriter("BEGIN EXCLUSIVE",false);
}

void testExplode(){
   COL_FUNCTION(testExplode);
   COLthreadPool* pBang;
   {
      COLthreadPool Bang;
      pBang = &Bang;
   }
   pBang->start();
}

TSTtestable* testSQLstatementIteratorTests()
{
   TST_START_COLLECTION("testSQLstatementIterator")
//      TST_ADD_TEST_FUNCTION(testSampleDataSetup);
      TST_ADD_TEST_FUNCTION(testSQLstatementIteratorBusy);
      TST_ADD_TEST_FUNCTION(testMultipleWriterThreads);
      //TST_ADD_TEST_FUNCTION(testExplode);
   TST_END_FIXTURE
}
