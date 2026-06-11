//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SIGexample.cpp
//
// Description:
//
// An example of using the SIG library.
//
// Author: Eliot Muir
// Date:   Thu 01/15/2004
//
//
//---------------------------------------------------------------------------

#include "SIGexamplePrecomp.h"
#pragma hdrstop

#include <COL/COLminimumInclude.h>
#include <CMD/CMDlineParser.h>

//tests
#include "testSIGsignaller.h"

//test framework
#include <TST/TSTconsoleApplication.h>

int main(int argc, const char** argv)
{
   try
   {
      TSTtestCollection Collection("SIG Unit Tests");

      // add all tests
      Collection.addTest(testSIGsignaller::signallerTests());

      CMDlineParser LineParser;

      // start console
      TSTconsoleApplication Application("SIG Unit Tests", Collection, LineParser);
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
