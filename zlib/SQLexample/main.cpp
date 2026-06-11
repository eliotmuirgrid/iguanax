//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Application: SQLexample 
//
// Description:
//
// SQLexample is a unit test program for the SQL library. 
// 
// Author: Eliot Muir
// Date:   Monday, July 25th, 2005 @ 08:35:55 PM
//
//---------------------------------------------------------------------------
#include <SQLexample/SQLexamplePrecomp.h>
#pragma hdrstop

#include <CMD/CMDlineParser.h>
#include <TST/TSTconsoleApplication.h>
#include <TST/TSTtestCollection.h>

#include <SQLexample/testSQLutils.h>

TSTtestable* testSQLsqliteTests();
TSTtestable* testSQLpreparedStatementTests();
TSTtestable* testSQLstatementIteratorTests();

int main(int argc, const char** argv)
{
   try
   {

      TSTtestCollection Collection("SQL Unit Tests");
      Collection.addTest(testSQLutils::testCollection());
      Collection.addTest(testSQLpreparedStatementTests());
      Collection.addTest(testSQLsqliteTests());
      Collection.addTest(testSQLstatementIteratorTests());

      CMDlineParser LineParser;

      TSTconsoleApplication Application("SQL Unit Tests", Collection, LineParser);
      LineParser.parseArgs(argc,argv);

      if (Application.run())
      {
         return EXIT_SUCCESS;
      }
      else
      {
         return EXIT_FAILURE;
      }
   }
   catch (const COLerror& Error)
   {
      COLcerr << Error.description() << newline;
      return EXIT_FAILURE;
   }
   catch (...)
   {
      COLcerr << "Unhandled Exception" << newline;
      return EXIT_FAILURE;
   }
   return EXIT_SUCCESS;
}
