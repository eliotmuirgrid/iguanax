//---------------------------------------------------------------------------
// Copyright (C) 1997-2005 iNTERFACEWARE Inc.  All Rights Reserved
//
// Application: ifware_register 
//
// Description:
//
// TODO
// 
// Author: Kevin Senn
// Date:   Friday, October 14th, 2005 @ 03:31:55 PM
//
//---------------------------------------------------------------------------
#include <ifware_register/Precomp.h>
#pragma hdrstop

#include <CMD/CMDlineParser.h>
#include <RGN/RGNmachineId.h>
#include <FIL/FILutils.h>
#include <CURL/CURLclient.h>
#include <CURL/CURLutils.h>

#include <ifware_register/AppVersion.h>

#define RCL_REGISTER_SERVER "registration.interfaceware.com"

#define RCL_FAKE_ID "F&A\\K\"=E'I\\n?D"

// Extracts the value XYZ from a String formatted as follows:
// Variable=XYZ
COLstring extractValue(const COLstring& String, const COLstring& Variable)
{
   COLstring SearchPattern = Variable + "=";
   COLuint32 StartIndex = String.find(SearchPattern) + SearchPattern.size();
   COLuint32 EndIndex = String.find("\n", StartIndex);
   return String.substr(StartIndex, EndIndex - StartIndex);
}

// Register Chameleon, unless the server returned an error
void updateRegistry(const COLstring& Response)
{
   if (Response.find("ERROR=") != npos) // server returned an error
   {
      COL_ERROR_STREAM_PLAIN("WARNING: Chameleon not registered.  See the above error message.", COLerror::PreCondition);
   }
   // else, we should register Chameleon
   COLstring ExpiryDate = extractValue(Response, "EXPIRY");
   COLstring Key = extractValue(Response, "KEY");
   COLuint32 DoesNotExpire = 0;

   if (ExpiryDate == "N/A")
   {
      ExpiryDate = "";
      DoesNotExpire = 1;
   }

   RGNsetRegistrationCode(Key.c_str(), ExpiryDate.c_str(), DoesNotExpire);
   try
   {
      RGNcheckLicense();
   }
   catch (const COLerror& Error)
   {
      COL_ERROR_STREAM_PLAIN("WARNING: Chameleon registration failed", COLerror::PreCondition);
   }

   COLcout << "Chameleon has been registered" << newline;
}

int main(int argc, const char** argv)
{
   try
   {
      COLcout << "ifware_register " << APP_VERSION_MAJOR << "." << APP_VERSION_MINOR << "." << APP_BUILD_NUMBER << newline << newline;
      CMDlineParser Parser;

      Parser.addFlagWithArgument("username", "username", "Register with specified username");
      Parser.addFlagWithArgument("password", "password", "The password corresponding to the username");
      Parser.addFlagWithArgument("type", "license-type", "The license type (an integer between 0 and 5).  If not specified, 3 (Chameleon Runtime License) will be used");
#ifdef _DEBUG
      // These flags are used only for debugging.  --fakeid tests that MySQL special characters
      // (such as \, ', ", and \n) are escaped properly, and also that URL special characters
      // (such as &, =, and ?) are escaped properly.  --host allows us to select the server
      // we wish to use.  This will likely be "localhost", when testing.
      COLstring FakeIdDescription;
      COLostream FakeIdStream(FakeIdDescription);
      FakeIdStream << "Use a fake machineID (" << RCL_FAKE_ID << ") to register.  This is for testing purposes only.";
      Parser.addFlagWithoutArgument("fakeid", FakeIdDescription.c_str());
      Parser.addFlagWithArgument("host", "hostname", "Use specified host (e.g., localhost).  This is for testing purposes only");
#endif // debug
      Parser.parseArgs(argc, argv);

      if (Parser.isFlagPresent("username") && Parser.isFlagPresent("password"))
      {
         COLstring UserName(Parser.flagArgument("username"));
         COLstring Password(Parser.flagArgument("password"));

         COLstring LicenseType;
         if (Parser.isFlagPresent("type"))
         {
            LicenseType = Parser.flagArgument("type");
         }
         else
         {
            LicenseType = "3"; // Chameleon runtime license is the default
         }

         COLstring MachineId(RGNmachineId());
#ifdef _DEBUG
         if (Parser.isFlagPresent("fakeid"))
         {
            MachineId = RCL_FAKE_ID;
         }
         COLstring DebugHost = RCL_REGISTER_SERVER;
         if (Parser.isFlagPresent("host"))
         {
            DebugHost = Parser.flagArgument("host");
         }
#endif // debug
         COLstring RegistrationKey;
         
         CURLclient Client;

         COLstring Request;
         COLostream RequestStream(Request);
         RequestStream << "http://";
#ifdef _DEBUG
         RequestStream << DebugHost;
#else
         RequestStream << RCL_REGISTER_SERVER;
#endif // debug
         RequestStream << "/register?" <<
            "username=" << CURLurlEscape(UserName) << "&" <<
            "password=" << CURLurlEscape(Password) << "&" <<
            "machineid=" << CURLurlEscape(MachineId) << "&" <<
            "licensetype=" << CURLurlEscape(LicenseType);

         COLstring RequestResult;
         COLuint64 HttpResponseCode = Client.sendRequest(Request, RequestResult);

         // Since we are using the default http port, if something goes wrong, we
         // could get some html back.  We don't want to display this to the command
         // prompt.
         if (RequestResult.find("<html>") != npos)
         {
            COLstring TitleTag("<title>");
            size_t IndexOfTitle = RequestResult.find(TitleTag) + TitleTag.size();
            size_t SizeOfTitle = RequestResult.find("</title>") - IndexOfTitle;
            COLcout << "ERROR=" << RequestResult.substr(IndexOfTitle, SizeOfTitle) << newline << newline;
         }
         else
         {
            // Print out result
            COLcout << RequestResult << newline;
         }

         // Update the registry, if errors did not occur
         updateRegistry(RequestResult);

         if (HttpResponseCode != CURLclient::HttpStatusOk)
         {
            COLcout << HttpResponseCode << newline;
            return EXIT_FAILURE;
         }
         return EXIT_SUCCESS;

      }
      else if (Parser.isFlagPresent("username") || Parser.isFlagPresent("password"))
      {
         // if they used at least one flag but not all
         COLcout << "You must use the password, username, and type flags." << newline;
         Parser.showUsage(COLcout);
         return EXIT_FAILURE;
      }

      if (Parser.unknownFlagPresent(COLcout))
      {
         Parser.showUsage(COLcout);
         return EXIT_FAILURE;
      }

      Parser.showUsage(COLcout);
      return EXIT_SUCCESS;
   }
   catch (const COLerror& Error)
   {
      COLcerr << "ERROR=" << Error.description() << newline;
      return EXIT_FAILURE;
   }
   catch (...)
   {
      COLcerr << "ERROR=Unhandled Exception" << newline;
      return EXIT_FAILURE;
   }
   return EXIT_SUCCESS;
}
