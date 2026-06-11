//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTexampleMain
//
// Description: Runs the tests
//
// Author: Nasron Cheong
// Date:   Febuary 2004 
//
//---------------------------------------------------------------------------

#include <TSTexample/TSTexamplePrecompiled.h>
#pragma hdrstop

#include <TSTexample/TSTtestClass2.h>
#include <TSTexample/TSTtestClass1.h>
#include <TSTexample/TSTtestSetUpTearDown.h>
#include <TST/TSTflags.h>
#include <TST/TSTconsoleApplication.h>

TSTtestable* TSTspecificationParserTests();

void registerTests()
{
   TSTtestCollection::globalCollection().addTest(tests1());
   TSTtestCollection::globalCollection().addTest(TSTtestClass2::tests2());

   //an actual test collection, we use TST to test its own parser
   TSTtestCollection::globalCollection().addTest(TSTspecificationParserTests());
   TSTtestCollection::globalCollection().addTest(TSTtestSetUpFunctionality::setUpTests());
   TSTtestCollection::globalCollection().addTest(TSTtestTearDownFunctionality::tearDownTests());
}


int main(int argc, const char** argv, const char* envp[])
{
   try
   {
      TSTtestCollection Collection("TST Unit Tests");
      Collection.addTest(tests1()); 
      Collection.addTest(TSTtestClass2::tests2()); 
      Collection.addTest(TSTspecificationParserTests()); 
      Collection.addTest(TSTtestSetUpFunctionality::setUpTests()); 
      Collection.addTest(TSTtestTearDownFunctionality::tearDownTests()); 

      // i know it looks strange having these tests added yet again, but the reason is because the tstParser test in TSTspecificationParserTests 
      // actually runs through the tests registered here in the global collection.  
      registerTests(); // this is necessary despite having added all the tests to the main test collection above.

      CMDlineParser LineParser;

      // start console
      TSTconsoleApplication Application("TST Unit Tests", Collection, LineParser);
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
   catch (COLerror& Error)
   {
      COLcerr << Error.description() << newline << flush;
      return EXIT_FAILURE;
   }
   catch(...)
   {
      COLcerr << "Unhandled Exception" << newline << flush;
      return EXIT_FAILURE;
   }
   return EXIT_SUCCESS;
}
