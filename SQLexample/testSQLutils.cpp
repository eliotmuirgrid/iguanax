//---------------------------------------------------------------------------
// Copyright (C) 1997-2005 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testSQLutils
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
#include <SQLexample/testSQLutils.h>

void testSQLutils::test()
{
   sqlite3* pSqlDatabase;
   int ReturnCode = sqlite3_open("test.db", &pSqlDatabase);
   
  
   sqlite3_close(pSqlDatabase);
}

TSTtestable* testSQLutils::testCollection()
{
   TST_START_FIXTURE(testSQLutils)
      TST_ADD_TEST_METHOD(test);
   TST_END_FIXTURE
}
