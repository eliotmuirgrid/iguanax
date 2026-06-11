//---------------------------------------------------------------------------
// Copyright (C) 1997-2004 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description: REXexample
//
// Implementation
// 
// Author: George Ma
// Date:   Tue 03/02/2004 
//
//---------------------------------------------------------------------------

#include <REXexample/REXexamplePrecomp.h>
#pragma hdrstop

#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFormatter.h>
#include <TST/TSTtestRunner.h>
#include <TST/TSTtestResult.h>
#include <TST/TSTtestResultFormatter.h>

TSTtestable* getREXmatcherTests();
TSTtestable* getREXfindTests();

//test framework
#include <TST/TSTconsoleApplication.h>
#include <CMD/CMDlineParser.h>

int main(int argc, const char** argv)     
{
   try
   {
      CMDlineParser LineParser;

      //add all tests
      TSTtestCollection Collection("REX Tests");
      Collection.addTest(getREXmatcherTests());
      Collection.addTest(getREXfindTests());

      //start console
      TSTconsoleApplication Application("REX Tests",Collection,LineParser);
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
   catch (...)
   {
      COLcerr << "Unhandled exception." << newline << flush;   
      return EXIT_FAILURE;
   }
  
   return EXIT_SUCCESS;
}
