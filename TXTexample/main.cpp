//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Application: TXTexample 
//
// Description:
//
// TXTexample is a unit test program for the TXT library. 
// 
// Author: Kevin Senn
// Date:   Wednesday, April 29th, 2009 @ 11:57:27 AM
//
//---------------------------------------------------------------------------
#include "TXTexamplePrecomp.h"
#pragma hdrstop

#include <CMD/CMDlineParser.h>
#include <TST/TSTconsoleApplication.h>
#include <TST/TSTtestCollection.h>

#include <TXTT/TXTTtestEncoding.h>
#include <TXTT/TXTTtestEncodingFilter.h>
#include <TXTT/TXTTtestTextSearch.h>
#include <TXTT/TXTTtestUtils.h>

//#include <TXT/TXTiconvDll.h>

int main(int argc, const char** argv)
{
   try
   {
      TSTtestCollection Collection("TXT Unit Tests");
      
      Collection.addTest(TXTTtestEncoding());
      Collection.addTest(TXTTtestEncodingFilter());
      Collection.addTest(TXTTtestTextSearch());
      Collection.addTest(TXTTtestUtils());

      CMDlineParser LineParser;

      TSTconsoleApplication Application("TXT Unit Tests", Collection, LineParser);
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
