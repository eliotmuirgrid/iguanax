//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testInit
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wed 14 Apr 2021 15:22:47 EDT
//---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>

#include <COL/COLlist.h>
#include <COL/COLostream.h>
#include <COL/COLutils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// #include <DB/DBdatabase#Factory.h>
#include <DB/DBdriverFind.h>
#include <DB/DBdriverValidate.h>

//MSSQL :1433 db = master user = sa pass = 49Psi7LL6#
//DB2 :50000 db = testdb user = db2inst1 pass = 49Psi7LL6#
//Mysql :3306 user = test pass 49Psi7LL6#                   // JULIAN db = test (???)
//Postgres :5432 user = postgres pass 49Psi7LL6#
//Oracle :1521 :55000  user = sys  pass  = Oradoc_db1

// static void testSimple(){
//    COL_FUNCTION(testSimple);
//    DBdatabase#Factory Factory;
//    COLstring Log;
//    COLostream Stream(Log);
//    Factory.initialize(Stream);
//    COLcout << Log;
// //   UNIT_ASSERT(1==0);
// }

static void testFindMySQL(){
   COL_FUNCTION(testFindMySQL);
   COLstring Log, Path;
   COLostream LogStream(Log);
   bool Success = DBfindMySql(&Path, LogStream);

   COLcout << Log << newline;
   COLcout << "Path: " << Path << newline;
   COLcout << "Success: " << Success << newline;
   // if (Success) {UNIT_ASSERT(DBmysqlValidate(LogStream, Path)); }
}

static void testFindMariaDb(){
   COL_FUNCTION(testFindMariaDb);
   COLstring Log, Path;
   COLostream LogStream(Log);
   bool Success = DBfindMariaDb(&Path, LogStream);

   COLcout << Log << newline;
   COLcout << "Path: " << Path << newline;
   COLcout << "Success: " << Success << newline;
   // if (Success) {UNIT_ASSERT(DBmariaDbValidate(LogStream, Path)); } // TODO - add code to only include maria db code when maria db support can be found on the machine.
}                                                                   // JULIAN - also with OTHER DB unit tests we only run them when a BD is found 

static void testFindOdbc(){
   COL_FUNCTION(testFindOdbc);
   COLstring Log, Path;
   COLostream LogStream(Log);
   bool Success = DBfindOdbc(&Path, LogStream);

   COLcout << Log << newline;
   COLcout << "Path: " << Path << newline;
   COLcout << "Success: " << Success << newline;
   // if (Success) {UNIT_ASSERT(DBodbcValidate(LogStream, Path)); }
   // UNIT_ASSERT(1==0);
}

static void testFindOracle(){
   COL_FUNCTION(testFindOracle);
   COLstring Log, Path;
   COLostream LogStream(Log);
   bool Success = DBfindOracleOci(&Path, LogStream);

   COLcout << Log << newline;
   COLcout << "Path: " << Path << newline;
   COLcout << "Success: " << Success << newline;
   // if (Success) {UNIT_ASSERT(DBoracleOciValidate(LogStream, Path)); }
   // UNIT_ASSERT(1==0);
}

void testInit(UNITapp* pApp){
   // pApp->add("init/simple", &testSimple);
   // pApp->add("init/find/mysql", &testFindMySQL);
   // pApp->add("init/find/mariadb", &testFindMariaDb);
   // pApp->add("init/find/odbc", &testFindOdbc);
   // pApp->add("init/find/oracle", &testFindOracle);
}
