//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IguanaLicense
//
// Description:
//
// A command-line utility to generate license keys for Iguana.
//
// Author: Henry Tran
// Date: July 27, 2004
//
//---------------------------------------------------------------------------

#include <FIL/FILfile.h>
#include <FIL/FILutils.h>

#include <DRX/DRXiguanaId.h>
#include <DRX/DRXinformation.h>
#include <DRX/DRXserialization.h>
#include <DRX/DRXutils.h>

#include <COL/COLerror.h>
#include <cstdio>
#include <CMD/CMDlineParser.h>

static void loadLicense(const COLstring& FileName, const COLstring& IguanaId, DRXinformation* pLicense){
   COLstring LicenseData;
   FILreadFile(FileName, &LicenseData);
   DRXparseLicense(LicenseData, IguanaId, pLicense);
}

static COLstring readLine(){
   char InputBuffer[1024];
   return ::fgets(InputBuffer,sizeof(InputBuffer),stdin);
}

static COLstring printExpiryDate(time_t Expiry){
   if (0 == Expiry) { return " (license doesn't expire)"; }
   time_t Now = time(nullptr);
   time_t TimeDifference = difftime(Expiry, Now); // time difference in seconds
   int DaysRemaining = TimeDifference / (24 * 60 * 60);
   COLstring Out = " (" + COLintToString(TimeDifference) + " days from now)";
   return Out;
}

static void printLicense(const DRXinformation& RegistrationInformation){
   COLcout << "Expiry             : "  << RegistrationInformation.expiryTimestamp()    << printExpiryDate(RegistrationInformation.expiryTimestamp())  << newline
           << "Component Limit    : "  << RegistrationInformation.componentLimit()                                                                    << newline
           << "Log Size Limit     : "  << RegistrationInformation.logSearchLimit()     << " Days"                                                     << newline
           << "Message Rate Limit : "  << RegistrationInformation.messageRateLimit()   << " per minute"                                               << newline
           << "Single Sign-On     : "  << RegistrationInformation.singleSignOn()                                                                      << newline
           << "High Availability  : "  << RegistrationInformation.highAvailability()                                                                  << newline
           << "Global Dashboard   : "  << RegistrationInformation.globalDashboard()                                                                   << newline;

}

int main( int argc, const char** argv ) {
   try {
      CMDlineParser Parser;
      Parser.addFlagWithArgument("print", "license_file", "Instead of generating a license, print the one provided");
      Parser.addFlagWithoutArgument("help","show usage.");
      Parser.parseArgs(argc, argv);
      if (Parser.isFlagPresent("help") || Parser.parsingErrorsPresent(COLcout)) {
         Parser.showUsage(COLcout);
         return EXIT_FAILURE;
      }

      COLcout << "Enter nothing to use default/existing values, which are displayed in [...]" << newline << newline;
      COLstring IguanaId = DRXiguanaId();
      COLcout << "Enter Iguana ID ["<< IguanaId <<  "]: ";
      COLstring InputIguanaId = readLine();
      InputIguanaId.replace( "\n", "" );
      if (!InputIguanaId.isWhitespace()) {
         IguanaId = InputIguanaId;
      }

      if (Parser.isFlagPresent("print")) {
         DRXinformation License;
         loadLicense(Parser.flagArgument("print"), IguanaId, &License);
         printLicense(License);
         return EXIT_SUCCESS;
      }

      COLstring UserInput;
      int DaysToExpiry = 0;
      COLcout << "Enter the number of days until the license expires (0 for non-expiring) [0]: ";
      UserInput = readLine();
      if (!UserInput.isWhitespace()) { DaysToExpiry = atoi(UserInput.c_str()); }
      int ComponentLimit = 0;
      COLcout << "Enter the number of components allowed (0 for unlimited) [0]: ";
      UserInput = readLine();
      if (!UserInput.isWhitespace()) { ComponentLimit = ::atoi(UserInput.c_str()); }
      int LogSearchLimit = 0;
      COLcout << "Enter the number of days back that logs can be searched (0 for unlimited) [0]: ";
      UserInput = readLine();
      if (!UserInput.isWhitespace()) { LogSearchLimit = ::atoi(UserInput.c_str()); }

      // Message rate is not yet tracked in the crm or mils
      int MessageRateLimit = 0;
      COLcout << "Enter the number of messages per minute that can be processed (0 for unlimited) [0]: ";
      UserInput = readLine();
      if (!UserInput.isWhitespace()) { MessageRateLimit = ::atoi(UserInput.c_str()); }

      bool SingleSignOn = true;
      COLcout << "Allow Single Sign-On (0 for OFF) [1]: ";
      UserInput = readLine();
      if (!UserInput.isWhitespace()) { SingleSignOn = atoi(UserInput.c_str()); }

      bool HighAvailability = true;
      COLcout << "Allow High Availability (0 for OFF) [1]: ";
      UserInput = readLine();
      if (!UserInput.isWhitespace()) { HighAvailability = atoi(UserInput.c_str()); }

      bool GlobalDashboard = true;
      COLcout << "Allow Global Dashboard (0 for OFF) [1]: ";
      UserInput = readLine();
      if (!UserInput.isWhitespace()) { GlobalDashboard = atoi(UserInput.c_str()); }

      DRXinformation License(DRXdaysToExpiryToTimestamp(DaysToExpiry), ComponentLimit, LogSearchLimit, MessageRateLimit, SingleSignOn, HighAvailability, GlobalDashboard);

      COLstring LicenseString;
      DRXserializeLicense(License, IguanaId, &LicenseString);
      COLcout << newline << "----------------------------------------" << newline;
      printLicense(License);
      COLcout << "----------------------------------------" << newline << newline;

      //print it in col width to make life easier for
      //cut and paste
      const char* pStart = LicenseString.c_str();
      const char* pEnd   = pStart + LicenseString.size();
      const COLuint32 Width = 40;
      while(pStart < pEnd) {
         COLuint32 WriteCount = COL_MIN(pEnd-pStart,Width);
         COLcout.sink()->write(pStart,WriteCount);
         COLcout << newline;
         pStart += WriteCount;
      }
      return EXIT_SUCCESS;
   } catch ( COLerror& Error ) {
      COLcerr << Error.description() << newline;
      return EXIT_FAILURE;
   } catch ( ... ) {
      COLcerr << "Unhandled exception." << newline;
      return EXIT_FAILURE;
   }
}
