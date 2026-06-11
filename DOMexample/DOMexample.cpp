//---------------------------------------------------------------------------
// Copyright (C) 1997-2004 iNTERFACEWARE Corp.  All Rights Reserved
//
// Module: DOMexample.cpp
//
// Description:
//
// An example of using the DOM library.
// 
// Author: Rob Moyse
// Date:   November 8, 2004
//---------------------------------------------------------------------------

#include <DOMexample/DOMexamplePreCompiled.h>
#pragma hdrstop

#include <stdio.h>
#include <stdlib.h>

#include <COL/COLerror.h>

#include <CMD/CMDlineParser.h>

#include <DOMexample/DOMexampleTests.h>

#include <TST/TSTtestCollection.h>
#include <TST/TSTconsoleApplication.h>

TSTtestable* testDOMbalanced();

int main(int argc, const char** argv)
{
   try {
      TSTtestCollection Collection("DOM Unit Tests");
      Collection.addTest(DOMexampleTests::allTests());
      Collection.addTest(testDOMbalanced());

      CMDlineParser LineParser;
      TSTconsoleApplication Application("DOM Unit Tests", Collection, LineParser);
      LineParser.parseArgs(argc,argv);

      if (Application.run()) {
         return EXIT_SUCCESS;
      } else {
         return EXIT_FAILURE;
      }
   }
   catch (COLerror& Error) {
      COLcerr << Error.description() << newline << flush;
      return EXIT_FAILURE;
   } catch(...) {
      COLcerr << "Unhandled Exception" << newline << flush;
      return EXIT_FAILURE;
   }
   return EXIT_SUCCESS;
}
