//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Application: NOXexample 
//
// Description:
//
// NODexample is a unit test program for the NOX library. 
// 
// Author: Kevin Senn
// Date:   Wednesday, September 22nd, 2010 @ 10:07:35 AM
//---------------------------------------------------------------------------

#include <CMD/CMDlineParser.h>
#include <TST/TSTconsoleApplication.h>
#include <TST/TSTtestCollection.h>

#include <NOXT/NOXTtestXml.h>

#include <FIL/FILutils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

int main(int argc, const char** argv){
   try{
      FILinitExecutableName(argv[0]); // for Posix daemons

      TSTtestCollection Collection("NOX Unit Tests");
      Collection.addTest(NOXTtestXml());
      CMDlineParser LineParser;
      TSTconsoleApplication Application("NOC Unit Tests", Collection, LineParser);
      LineParser.parseArgs(argc,argv);
      if (Application.run()){
         return EXIT_SUCCESS;
      } else{
         return EXIT_FAILURE;
      }
   } catch (const COLerror& Error) {
      COLcerr << Error.description() << newline;
      return EXIT_FAILURE;
   } catch (...) {
      COLcerr << "Unhandled Exception" << newline;
      return EXIT_FAILURE;
   }
   return EXIT_SUCCESS;
}
