//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Tests for XML
//
// Author: Nasron Cheong
// Date:   Nov 2003
//
//---------------------------------------------------------------------------

#include <XML/XMLiosStream.h>
#include <COL/COLstring.h>
#include <COL/COLsinkStandard.h>

#include "testXMLiosStream.h"
#include <COL/COLostream.h>
#include <COL/COLerror.h>

//test framework
#include <TST/TSTtestCollection.h>
#include <TST/TSTconsoleApplication.h>
#include <CMD/CMDlineParser.h>

#include <stdlib.h>

TSTtestable* getXMLparserTests();
TSTtestable* getXMLansiParserTests();
//TSTtestable* getXMLexpatWrapperTests();

int main(int argv, const char** argc)
{
   try
   {
      TSTtestCollection Collection("XML tests");

      CMDlineParser LineParser;
      TSTconsoleApplication Application("XMLexample",Collection,LineParser);
      Collection.addTest( testXMLiosStream::getTests() );
      Collection.addTest( getXMLparserTests() );
      Collection.addTest( getXMLansiParserTests() );
      //      Collection.addTest(getXMLexpatWrapperTests());

      LineParser.parseArgs(argv,argc);

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
   }
   catch(...)
   {
      COLcerr << "Unhandled Exception" << newline << flush;
   }
   return EXIT_FAILURE;
}
