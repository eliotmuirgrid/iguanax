//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Application: TSMexample 
//
// Description:
//
// TSMexample is a unit test program for the TSM library. 
// 
// Author: Matt Lawrence
// Date:   Tuesday, February 27th, 2007 @ 08:50:35 AM
//
//---------------------------------------------------------------------------
#include "TSMexamplePrecomp.h"
#pragma hdrstop

#include <CMD/CMDlineParser.h>
#include <TST/TSTconsoleApplication.h>
#include <TST/TSTtestCollection.h>

#include "testTSMattribute.h"
#include "testTSMattributeList.h"
#include "testTSMhandlerStack.h"
#include "testTSMlabel.h"
#include "testTSMvariant.h"
#include "testTSMwriteBuffer.h"

int main(int argc, const char** argv)
{
   try
   {
      TSTtestCollection Collection("TSM Unit Tests");
      Collection.addTest(testTSMattribute::testCollection());
      Collection.addTest(testTSMattributeList::testCollection());
      Collection.addTest(testTSMhandlerStack::testCollection());
      Collection.addTest(testTSMlabel::testCollection());
      Collection.addTest(testTSMvariant::testCollection());
      Collection.addTest(testTSMwriteBuffer::testCollection());

      CMDlineParser LineParser;

      TSTconsoleApplication Application("TSM Unit Tests", Collection, LineParser);
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
   catch (const COLerror& Error)
   {
      COLcerr << Error.description() << newline;
      return EXIT_FAILURE;
   }
   catch (...)
   {
      COLcerr << "Unhandled Exception" << newline;
      return EXIT_FAILURE;
   }
   return EXIT_SUCCESS;
}
