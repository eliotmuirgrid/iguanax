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

#include "COLexamplePrecomp.h"
#pragma hdrstop

#include <COL/COLdebugNewDelete.h>

#include <COL/COLminimumInclude.h>
#include <COL/COLostream.h>
#include <CMD/CMDlineParser.h>

#include <COLT/COLTvarUtils.h>

//tests
#include "testCOLarray.h"
#include "testCOLlist.h"
#include "testCOLhashmap.h"
#include "testCOLmap.h"
#include "testCOLstrerror.h"
#include "testCOLdateTime.h"
#include "testCOLmutex.h"
#include "testCOLthreadLocal.h"

TSTtestable* COLstringTests();
TSTtestable* COLostreamTests();
TSTtestable* COLownerPtrTests();
TSTtestable* COLsingletonTests();
TSTtestable* COLmathTests();
TSTtestable* COLutilsTests();
TSTtestable* COLdllTests();
TSTtestable* COLerrorTests();
TSTtestable* COLendianTests();
TSTtestable* COLvarTests();
TSTtestable* COLthreadPoolTests();
TSTtestable* COLclosureTests();
TSTtestable* COLthreadLocalTests();

//test framework
#include <TST/TSTconsoleApplication.h>

int main(int argc, const char** argv)
{
   try
   {
      TSTtestCollection Collection("COL Unit Tests");

      // add all tests
      Collection.addTest(testCOLdateTime::COLdateTimeTests());
      Collection.addTest(COLarrayTests());
      Collection.addTest(testCOLlist::COLlistTests());
      Collection.addTest(testCOLhashmap::COLhashmapTests());
      Collection.addTest(testCOLmap::COLmapTests());
      Collection.addTest(COLstringTests());
      Collection.addTest(COLostreamTests());
      Collection.addTest(COLstrerrorTests());
      Collection.addTest(COLmathTests());
      Collection.addTest(COLutilsTests());
      Collection.addTest(COLownerPtrTests());
      Collection.addTest(COLdllTests());
      Collection.addTest(COLerrorTests());
      Collection.addTest(COLendianTests());
      Collection.addTest(COLTvarUtils());
      Collection.addTest(COLthreadPoolTests());
      Collection.addTest(COLvarTests());
      Collection.addTest(COLclosureTests());
      Collection.addTest(COLmutexTests());
      Collection.addTest(COLthreadLocalTests());

      CMDlineParser LineParser;

      // start console
      TSTconsoleApplication Application("COL Unit Tests", Collection, LineParser);
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
