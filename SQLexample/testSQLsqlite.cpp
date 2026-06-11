//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
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


#include <COL/COLevent.h>
#include <COL/COLdateTime.h>
#include <COL/COLlog.h>
#include <COL/COLutils.h>
COL_LOG_MODULE;

#include <SQL/SQLsqlite.h>
#include <SQL/SQLtransaction.h>
#include <SQL/SQLpreparedStatement.h>
#include <SQL/SQLstatementIterator.h>

#include "testSQLutils.h"

#include <COL/COLthreadPool.h>

#ifdef _WIN32
#include <sys/timeb.h>
COLuint64 currentTime()
{
   struct _timeb Time;
   _ftime(&Time);
   return (COLuint64)Time.time * 1000 + Time.millitm;
}
#else
#include <unistd.h>
#include <sys/time.h>
COLuint64 currentTime()
{
   struct timeval Time;
   gettimeofday(&Time, 0);
   return (COLuint64)Time.tv_sec * 1000 + Time.tv_usec / 1000;
}
#endif

// make file name global to use the same database in the two functions
const char* gpFileName = "insert_speed_test.db";

#include <SQL/SQLpreparedStatement.h>
#include <SQL/SQLstatementIterator.h>

void testSQLsqlLiteInsertSpeedTest()
{
   // add some data to the insertion test
   const char* pData = 
      "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
      "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
      "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
      "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
      "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
      "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
      "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
      "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
      "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
      "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"

      "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
      "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
      "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
      "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
      "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
      "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
      "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
      "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
      "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
      "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
      ;
   if (FILfileExists(gpFileName))
   {
      FILremoveWithThrow(gpFileName);
   }

   SQLsqlite Database;
   Database.open(gpFileName);
   
   Database.executeSqlString("CREATE TABLE INSERT_TEST(InsertIndex INTEGER PRIMARY KEY, InsertData TEXT);");
   
   COLdateTime StartTime = COLdateTime::currentTime();
   const COLuint32 TotalInserts = 10000;
   COLuint32 CountOfInsert = TotalInserts;
   // do all insertion in the one transaction - 100 times faster!!!
   // speed is about 800 insertions per second!!!
   SQLtransaction Transaction(Database);
   while(CountOfInsert)
   {
      COLstring ExecuteStatement;
      COLostream ExecuteStream(ExecuteStatement);
      ExecuteStream << "INSERT INTO INSERT_TEST (InsertIndex,InsertData) VALUES(" << CountOfInsert << ", '" 
         << pData << "');"; 
      Database.executeSqlString(ExecuteStatement.c_str());
      CountOfInsert--;
   }
   Transaction.commit();
   COLdateTime EndTime = COLdateTime::currentTime();
   COL_TRC("Inserts : " << TotalInserts);
   COL_TRC("Seconds : " << (EndTime-StartTime).totalSeconds());
   COL_TRC("Inserts Per Second: " << (double)(((double)TotalInserts) / (EndTime - StartTime).totalSeconds()));
   Database.close();
}

// speed test of "select total number of records up to an index"
void testSQLsqlLiteCountToIndexSpeedTest()
{
   SQLsqlite Database;
   // open the database which was filled in the previous test
   Database.open(gpFileName);

   COLuint64 StartTime = currentTime();

   SQLpreparedStatement SelectCountOfItemToIndex;
   // select total number of records up to an index
   SelectCountOfItemToIndex.prepare( Database, "SELECT COUNT(*) FROM INSERT_TEST WHERE InsertIndex <= ?;");
   COLuint64 PrepareTime = currentTime();
   
   SQLtransaction Transaction( Database );
   SelectCountOfItemToIndex.bindInteger64Value(1, 10000);
   SQLstatementIterator Iterator(SelectCountOfItemToIndex);   
   Iterator.stepNext();
   COLuint64 CountOfItem = Iterator.integer64Value(0);
   SelectCountOfItemToIndex.finalize();
   Transaction.commit();
   
   COLuint64 EndTime = currentTime();
   COL_TRC("Count(*): " << CountOfItem);
   COL_TRC("Time for prepare: " << PrepareTime - StartTime << " ms");
   COL_TRC("Time for count(*) to index: " << EndTime - PrepareTime << " ms");
   Database.close();
}


class SQLtestContentionDatabaseReader {
public:
   SQLtestContentionDatabaseReader(const COLstring& iDatabaseName) : DatabaseName(iDatabaseName), IsStopRequested(false) {}
   ~SQLtestContentionDatabaseReader() {}
   
   void run() {
      try {
         SQLsqlite Database;
         Database.open(DatabaseName); 
         while (!isStopRequested()) {
            SQLpreparedStatement Statement;
            Statement.prepare(Database,"select * from TestTable");
            SQLstatementIterator Iterator(Statement);
            for (Iterator; Iterator.stepNext();) {
               Iterator.integer64Value(1);
               const char* pOut = 0;
               Iterator.textValue(2,&pOut);
               COLsleep(10);
            }                                
         }
      } catch(const COLerror& Error) {
         COL_TRC(Error);
      }
      Done.signal();
   }
   
   bool isStopRequested() { return IsStopRequested; }

   COLevent Done;
   bool IsStopRequested;
   const COLstring DatabaseName;
};

void testSQLsqlLiteContentionTest() {
   COL_FUNCTION(testSQLsqlLiteContentionTest);

   const char* pFileName = "contention_test.db";
   if (FILfileExists(pFileName)) {
      FILremoveWithThrow(pFileName);
   }
   
   {
      SQLsqlite Database;
      Database.open(pFileName);
      Database.executeSqlString("CREATE TABLE TestTable(TestIndex INTEGER PRIMARY KEY, TestData TEXT);");
   }
   
   COLthreadPool Pool;
   Pool.start();
   SQLtestContentionDatabaseReader Reader(pFileName);
   Pool.scheduleTask(COLnewClosure0(&Reader, &SQLtestContentionDatabaseReader::run));

   try {
      SQLsqlite Database;
      Database.open(pFileName);
      const COLuint32 TotalInserts = 5;
      COLuint32 CountOfInsert = TotalInserts;
      while(CountOfInsert) {
         SQLtransaction Transaction(Database);
         COL_VAR(CountOfInsert);
         COLstring ExecuteStatement;
         COLostream ExecuteStream(ExecuteStatement);
         ExecuteStream << "INSERT INTO TestTable (TestIndex,TestData) VALUES(" << CountOfInsert << ",\"\")"; 
         Database.executeSqlString(ExecuteStatement.c_str());
         CountOfInsert--;
         Transaction.commit();
      }
      Reader.IsStopRequested = true; //Reader is freed before scheduled task is completed resulting in memory use after free
      Reader.Done.wait();
      COLsleep(100);
   } catch(...) {
      COL_TRC("Caught an error");
      Reader.IsStopRequested = true;
      Reader.Done.wait();
      COLsleep(100);     
      throw;
   }
}

//See #4141
void testSQLsqlLiteFileReopen()
{
   const char* pFileName = "reopen_test.db";
   if (FILfileExists(pFileName))
   {
      FILremoveWithThrow(pFileName);
   }
   SQLsqlite Database;
   Database.open(pFileName);
   Database.open(pFileName);
   TST_ASSERT(Database.filePath() == pFileName)

   Database.close();
   TST_ASSERT(Database.filePath() == "");
   Database.close();
}

static void testSQLsqlLiteBusyTest()
{
   SQLsqlite Database;
   const char* pFileName = "busy_test.db";
   if (FILfileExists(pFileName))
   {
      FILremoveWithThrow(pFileName);
   }

   //just for read
   Database.open(pFileName);

   //open a write transaction to hold an exclusive lock, on a separate database connection
   SQLsqlite Database2;
   Database2.open(pFileName);
   SQLexclusiveTransaction TransactionEx(Database2);
   {
      testSQLiteratorBusyHandler BusyHandler;
      BusyHandler.reset(10);
      BusyHandler.setSqliteBusyInterval(10);
      
      //try to acquire a read lock with a different db      
      bool Result = Database.executeSqlStringBusy("BEGIN IMMEDIATE;",BusyHandler);
      COLPRECONDITION(!Result); //must have interrupted
      COLPRECONDITION(BusyHandler.CountOfCalled == 10);
   }
}

static const COLstring Letters[] = {"three", "eight", "nineteen"};
static const COLstring Numbers[] = {    "3",     "8",       "19"};

static bool iterateGoodTable(SQLsqlite& Database) {
   SQLpreparedStatement Statement;
   int Ticker = 0;
   try {
      Statement.prepare(Database,"SELECT * from good_table");
      for (SQLstatementIterator Iterator(Statement); Iterator.stepNext();) {
         TST_ASSERT(COLintToString(Iterator.integerValue(0)) == Numbers[Ticker])
         TST_ASSERT(Iterator.textValueStr(1) == Letters[Ticker]);
         Ticker++;
      }
   } catch(const COLerror& Error) {
      const COLstring& Description = Error.description();
      TST_ASSERT(Description.find("file is encrypted or is not a database") != npos);
      return false;
   }
   return true;
}

void testSQLsqliteEncryption() {
   const COLstring FileName("ecrypted.db");
   if (FILfileExists(FileName)) {
      FILremoveWithThrow(FileName);
   }
   SQLsqlite Database, Database1, Database2, Database3;
   const COLstring EncryptionKey("big_secret");
   Database.open(FileName, EncryptionKey);
   Database.executeSqlString("CREATE TABLE IF NOT EXISTS good_table(numbers INTEGER, letters TEXT);");
   SQLexclusiveTransaction Transaction(Database);
   for (int i = 0; i < 3; i++) {
      COLstring ExecuteStatement;
      COLostream ExecuteStream(ExecuteStatement);
      ExecuteStream << "INSERT INTO good_table VALUES(" << Numbers[i] << ", '" << Letters[i] << "')";
      Database.executeSqlString(ExecuteStatement.c_str());
   }
   Transaction.commit();
   Database.close();
   Database1.open(FileName);
   TST_ASSERT(! iterateGoodTable(Database1));
   Database1.close();
   Database2.open(FileName, EncryptionKey);
   TST_ASSERT(iterateGoodTable(Database2));
   Database2.close();
   Database3.open(FileName, "bad_secret");
   TST_ASSERT(! iterateGoodTable(Database3));
   Database3.close();
}

TSTtestable* testSQLsqliteTests()
{
   TST_START_COLLECTION("testSQLsqlite")
      TST_ADD_TEST_FUNCTION(testSQLsqlLiteFileReopen);
      TST_ADD_TEST_FUNCTION(testSQLsqlLiteContentionTest);
      // Speed tests don't need to be run in normal usage. Uncomment if you
      // would like to run them.
      //TST_ADD_TEST_FUNCTION(testSQLsqlLiteInsertSpeedTest);
      //TST_ADD_TEST_FUNCTION(testSQLsqlLiteCountToIndexSpeedTest);
      TST_ADD_TEST_FUNCTION(testSQLsqlLiteBusyTest);
      // Our SQLite library does not support encryption anymore.
      //TST_ADD_TEST_FUNCTION(testSQLsqliteEncryption);
   TST_END_FIXTURE
}
