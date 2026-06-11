//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Application: TIMEexample 
//
// Description:
//
// TIMEexample is a unit test program for the TIME library. 
// 
// Author: Dmitri Klimenko
// Date:   Tuesday, October 5th, 2010 @ 06:38:19 PM
//
//---------------------------------------------------------------------------
#include "TIMEexamplePrecomp.h"
#pragma hdrstop

#include <CMD/CMDlineParser.h>
#include <TST/TSTconsoleApplication.h>
#include <TST/TSTtestCollection.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <TIMET/TIMETdate.h>
#include <TIMET/TIMETts.h>

int main(int argc, const char** argv)
{
   try
   {
      TSTtestCollection Collection("TIME Unit Tests");
      Collection.addTest(TIMETdateTests());
      Collection.addTest(TIMETtsTests());

      CMDlineParser LineParser;

      TSTconsoleApplication Application("TIME Unit Tests", Collection, LineParser);
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
