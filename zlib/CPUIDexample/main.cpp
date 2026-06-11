//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Application: CPUIDexample 
//
// Description:
//
// CPUIDexample is a unit test program for the CPUID library. 
// 
// Author: Steven Dodd
// Date:   Friday, September 10th, 2010 @ 10:10:44 AM
//
//---------------------------------------------------------------------------
#include "CPUIDexamplePrecomp.h"
#pragma hdrstop

#include <CMD/CMDlineParser.h>
#include <TST/TSTconsoleApplication.h>
#include <TST/TSTtestCollection.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <CPUIDT/CPUIDTquery.h>

int main(int argc, const char** argv)
{
   try
   {
      TSTtestCollection Collection("CPUID Unit Tests");
      Collection.addTest(CPUIDTquery());

      CMDlineParser LineParser;

      TSTconsoleApplication Application("CPUID Unit Tests", Collection, LineParser);
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
