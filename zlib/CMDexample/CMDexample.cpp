//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMDexample.cpp
//
// Description:
//
// An example of using the CMD library.
// 
// Author: Roman Leifer
// Date:   Tue 08/17/2004 
//
//
//---------------------------------------------------------------------------

#include <CMDexample/CMDexamplePrecomp.h>
#pragma hdrstop

#include <CMDexample/CMDtest.h>


int main(int argc, const char** argv)
{
   try
   {

      TSTtestCollection Collection("CMD Unit Tests");
      
      // add all tests
      Collection.addTest(CMDtest::tests());
      Collection.addTest(CMDTlineParserTests());
      
      CMDlineParser LineParser;

      // start console
      TSTconsoleApplication Application("CMD Unit Tests", Collection, LineParser);
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
      COLcerr << "Unhandled Exception" << newline << flush;
      return EXIT_FAILURE;
   }
   return EXIT_SUCCESS;
}
