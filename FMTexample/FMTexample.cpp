//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: Main function for FMTexample
//
// Description:
//
// An example of using the FMT library.
//
// Author: Yunqi Zhang
// Date:   Wed 29/09/2004 
//
//
//---------------------------------------------------------------------------

#include <FMTexample/FMTexamplePrecomp.h>
#pragma hdrstop

#include <CMD/CMDlineParser.h>
#include <TST/TSTconsoleApplication.h>
#include <TST/TSTtestCollection.h>


TSTtestable* FMTbinaryDumpTest();
TSTtestable* FMTlineFeedConvertTest();
TSTtestable* DISbinaryDisplayTest();

int main(int argc, const char** argv)
{
   try
   {     
      CMDlineParser LineParser;
      TSTtestCollection Collection("FMT Unit Test");
           
      TSTconsoleApplication Application("FMT Unit Test", Collection, LineParser);
      
      Collection.addTest(FMTbinaryDumpTest());
      Collection.addTest(FMTlineFeedConvertTest());
      Collection.addTest(DISbinaryDisplayTest());

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
