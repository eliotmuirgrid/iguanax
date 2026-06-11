//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Application: SCKBexample
//
// Description:
//
// SCKBexample is a unit test program for the SCKB library.
//
// Author: John Q
// Date:   Tue  1 Nov 2022 22:08:55 EDT
//---------------------------------------------------------------------------
#include <stdlib.h>

#include <CMD/CMDlineParser.h>
#include <COL/COLerror.h>
#include <SCK/SCKutils.h>
#include <TST/TSTconsoleApplication.h>
#include <TST/TSTtestCollection.h>

TSTtestable* testSslContext();
TSTtestable* testConnection();
TSTtestable* testSslConnection();

int main(int argc, const char** argv) {
   try {
      SCKinitWinsock();
      TSTtestCollection Collection("SCKB Unit Tests");
      Collection.addTest(testSslContext());
      Collection.addTest(testConnection());
      Collection.addTest(testSslConnection());

      CMDlineParser LineParser;
      TSTconsoleApplication Application("SCKB Unit Tests", Collection, LineParser);
      LineParser.parseArgs(argc,argv);

      return Application.run() ? EXIT_SUCCESS : EXIT_FAILURE;
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
