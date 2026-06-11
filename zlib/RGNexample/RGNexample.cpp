//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description: RGNexample
//
// Implementation
// 
// Author: George Ma
// Date:   Tue 03/02/2004 
//
//---------------------------------------------------------------------------

#include "RGNexamplePrecomp.h"
#pragma hdrstop

#include <RGN/RGNmachineId.h>

#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFormatter.h>
#include <TST/TSTtestRunner.h>
#include <TST/TSTtestResult.h>
#include <TST/TSTtestResultFormatter.h>

#include <COL/COLlog.h>
#include <PRO/PROenv.h>
COL_LOG_MODULE;

TSTtestable* getRGNTests();


//test framework
#include <TST/TSTconsoleApplication.h>
#include <CMD/CMDlineParser.h>

int main(int argc, const char** argv)     
{
   try
   {

      CMDlineParser LineParser;

      //add all tests
      TSTtestCollection Collection("RGN Unit Test");
      Collection.addTest(getRGNTests());

      //start console
      TSTconsoleApplication Application("RGN Unit Test",Collection,LineParser);
      LineParser.addFlagWithoutArgument("showenv", "Show Machine ID and License for this system");
      LineParser.parseArgs(argc,argv);

      COLstring MachineID = RGNmachineId();
      COLstring License = RGNlicenseKey(MachineID, "", 1 );

      if(LineParser.isFlagPresent("showenv")){
         COLcout << "# Machine ID: " << MachineID << newline;
         COLcout << "export CHM_REGKEY=" << License << newline;
         COLcout << "export CHM_EXPIRY_DATE=NOEXPIRY" << newline;
         return EXIT_SUCCESS;
      };

      // Don't use COL_TRC until the command line arguments have been parsed!
      COL_TRC( "Your Machine ID: " << MachineID);
      COL_TRC("Your License (NOEXPIRY): " << License);

      // Make sure that the host machine has a valid Chameleon license before running
      // the tests.
      COLstring LicenseKey = RGNextensionKey(MachineID, "", true);
#ifdef _WIN32
      RGNsetRegistrationCode(LicenseKey.c_str(), "", true);
#else
      PROsetenv("CHM_REGKEY", LicenseKey);
      PROsetenv("CHM_EXPIRY_DATE", "NOEXPIRY");
#endif

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
      COLcerr << "Unhandled exception." << newline << flush;   
      return EXIT_FAILURE;
   }
   
   return EXIT_SUCCESS;
}
