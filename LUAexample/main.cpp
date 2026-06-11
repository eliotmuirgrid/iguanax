//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Application: LUAexample 
//
// Description:
//
// LUAexample is a unit test program for the LUA library. 
// 
// Author: Eliot Muir
// Date:   Friday, April 23rd, 2010 @ 11:57:46 AM
//
//---------------------------------------------------------------------------
#include "LUAexamplePrecomp.h"
#pragma hdrstop

#include <CMD/CMDlineParser.h>
#include <TST/TSTconsoleApplication.h>
#include <TST/TSTtestCollection.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <LUAT/LUAThello.h>
#include <LUAT/LUATlexer.h>
#include <LUAT/LUATtestLineParser.h>
#include <LUAT/LUATreturn.h>
#include <LUAT/LUATupvalues.h>
#include <LUAT/LUATloops.h>
#include <LUAT/LUATvar.h>

int main(int argc, const char** argv)
{
   try
   {
      TSTtestCollection Collection("LUA Unit Tests");

      Collection.addTest(LUAThello());
      Collection.addTest(LUATlexer());
      Collection.addTest(LUATtestLineParser());
      Collection.addTest(LUATdoesChunkReturn());
      Collection.addTest(LUATupvalues());
      Collection.addTest(LUATloops());
      Collection.addTest(LUATluaToVar());
      CMDlineParser LineParser;

      TSTconsoleApplication Application("LUA Unit Tests", Collection, LineParser);
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
