//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Application: HTTPexample 
//
// Description:
//
// HTTPexample is a unit test program for the HTTP library. 
// 
// Author: Eliot Muir
// Date:   Tuesday, December 12th, 2006 @ 02:40:10 PM
//---------------------------------------------------------------------------
#include "HTTPexamplePrecomp.h"
#pragma hdrstop

#include <CMD/CMDlineParser.h>
#include <TST/TSTconsoleApplication.h>
#include <TST/TSTtestCollection.h>

#include <FIL/FILutils.h>

#include "testHTTPutils.h"

TSTtestable* testHTTPparser();
TSTtestable* testHTTPcookie();

int main(int argc, const char** argv)
{
   try
   {     
      FILinitExecutableName(argv[0]);
      TSTtestCollection Collection("HTTP Unit Tests");
      Collection.addTest(testHTTPutils::testCollection());
      Collection.addTest(testHTTPparser());
      Collection.addTest(testHTTPcookie());

      CMDlineParser LineParser;

      TSTconsoleApplication Application("HTTP Unit Tests", Collection, LineParser);
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
