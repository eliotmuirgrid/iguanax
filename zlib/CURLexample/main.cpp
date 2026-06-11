//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Application: CURLexample
//
// Description:
//
// TODO
//
// Author: Kevin Senn
// Date:   Thursday, January 12th, 2006 @ 03:19:05 PM
//
//---------------------------------------------------------------------------
#include <CMD/CMDlineParser.h>
#include <TST/TSTconsoleApplication.h>
#include <TST/TSTtestCollection.h>
#include <CURLexample/testCURLutils.h>
#include <CURL/CURLutils.h>
#include <FIL/FILutils.h>

int main(int argc, const char** argv) {
   try {
      CURLglobalInit(FILworkingDir() + "cacert.pem");
      TSTtestCollection Collection("CURL Library Unit Tests");
      Collection.addTest(testCURLutils::testCollection());

      CMDlineParser LineParser;
      TSTconsoleApplication Application("CURL Unit Tests", Collection, LineParser);
      LineParser.parseArgs(argc,argv);
      return Application.run() ? EXIT_SUCCESS : EXIT_FAILURE;
   } catch (const COLerror& Error) {
      COLcerr << Error.description() << newline;
      return EXIT_FAILURE;
   } catch (...) {
      COLcerr << "Unhandled Exception" << newline;
      return EXIT_FAILURE;
   }
   return EXIT_SUCCESS;
}
