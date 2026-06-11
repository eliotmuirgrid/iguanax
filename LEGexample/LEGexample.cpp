//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLexample.cpp
//
// Description:
//
// An example of using the COL library.
//
// Author: Eliot Muir
// Date:   Thu 01/15/2004
//
//
//---------------------------------------------------------------------------

#include "LEGexamplePrecomp.h"
#pragma hdrstop

#include <COL/COLminimumInclude.h>
#include <COL/COLostream.h>
#include <CMD/CMDlineParser.h>

#include <SCK/SCKutils.h>

// Tests
#include "testLEGvector.h"

//test framework
#include <TST/TSTconsoleApplication.h>

int main(int argc, const char** argv)
{
   try
   {
      SCKinitWinsock();
      TSTtestCollection Collection("COL Unit Tests");
      
      Collection.addTest(LEGvectorTests());

      CMDlineParser LineParser;

      // start console
      TSTconsoleApplication Application("LEG Unit Tests", Collection, LineParser);
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
