//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Application: NODexample 
//
// Description:
//
// NODexample is a unit test program for the NOD library. 
// 
// Author: Kevin Senn
// Date:   Wednesday, September 22nd, 2010 @ 10:07:35 AM
//
//---------------------------------------------------------------------------

#include <CMD/CMDlineParser.h>
#include <TST/TSTconsoleApplication.h>
#include <TST/TSTtestCollection.h>

#include <NODT/NODTtestPlace.h>
#include <NODT/NODTtestAddress.h>
#include <NODT/NODTtestOperations.h>


#include <NODBT/NODBTtestResultSet.h>

#include <NOBT/NOBTtest.h>

#include <FIL/FILutils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

int main(int argc, const char** argv) {
   try {
      FILinitExecutableName(argv[0]); // for Posix daemons

      TSTtestCollection Collection("NOD Unit Tests");

      // Generic NOD tests
      Collection.addTest(NODTtestPlace());
      Collection.addTest(NODTtestAddress());
      Collection.addTest(NODTtestOperations());

      // DB
      Collection.addTest(NODBTtestResultSet());

      // Binary serialization
      Collection.addTest(NOBTtest());

      CMDlineParser LineParser;

      TSTconsoleApplication Application("NOD Unit Tests", Collection, LineParser);
      LineParser.parseArgs(argc,argv);

      if (Application.run()) {
         return EXIT_SUCCESS;
      } else {
         return EXIT_FAILURE;
      }
   }
   catch (const COLerror& Error) {
      COLcerr << Error.description() << newline;
      return EXIT_FAILURE;
   }
   catch (...) {
      COLcerr << "Unhandled Exception" << newline;
      return EXIT_FAILURE;
   }
   return EXIT_SUCCESS;
}
