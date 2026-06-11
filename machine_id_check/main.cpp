//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Application: machine_id_check
//
// Description:
//
// TODO
//
// Author: Art Harrison
// Date:   Wednesday, June 9th, 2010 @ 03:00:12 PM
//
//---------------------------------------------------------------------------
#include "Precomp.h"
#pragma hdrstop

#include <CMD/CMDlineParser.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include "AppVersion.h"

#include <RGN/RGNmachineId.h>
#include <IP/IPutils.h>
#include <FIL/FILutils.h>
#include <COL/COLdateTime.h>
#include <REG/REGregistryWindows.h>

COLstring RGNmacAddress();

#include "MICmachineId.h"
#include <DBE/DBEencryptString.h>

COLstring MICcharToHex(char Char)
{
   char Buffer[16];
   snprintf(Buffer, 16, "\\x%02x", (unsigned)(unsigned char)Char);
   return Buffer;
};

COLstring MICstringToHex(const COLstring& String)
{
   COLstring Result;
   for(COLindex CharIndex=0; CharIndex < String.size(); CharIndex++)
   {
      Result += MICcharToHex(String[CharIndex]);
   };
   return Result;
};

extern COLstring RGNsoftwareRegistryPath(const COLstring& SubPath);

extern const char* CHMmachineIdSubPath;


int main(int argc, const char** argv)
{
   try
   {
      COLcout << "machine_id_check " << APP_VERSION_MAJOR << '.' << APP_VERSION_MINOR << '.' << APP_BUILD_NUMBER << newline;
      BIDwriteBuildInfo(COLcout,true);
      COLcout << "64-bit mode" << newline;
      COLcout << newline;

#ifdef _WIN32
      OSVERSIONINFOEX OsVersionInfo;
      memset(&OsVersionInfo, 0, sizeof(OsVersionInfo));
      OsVersionInfo.dwOSVersionInfoSize = sizeof(OsVersionInfo);
      GetVersionEx((OSVERSIONINFO*)&OsVersionInfo);
      COLcout << "Windows version: " << OsVersionInfo.dwMajorVersion << "." << OsVersionInfo.dwMinorVersion << " build " << OsVersionInfo.dwBuildNumber << " " << OsVersionInfo.szCSDVersion << newline;

#ifdef CHM_VC9
      SYSTEM_INFO SystemInfo;
      memset(&SystemInfo, 0, sizeof(SystemInfo));
      GetNativeSystemInfo(&SystemInfo);
      const char* Architecture = "Other";
      switch(SystemInfo.wProcessorArchitecture)
      {
      case PROCESSOR_ARCHITECTURE_AMD64:
         Architecture = "AMD64";
         break;
      case PROCESSOR_ARCHITECTURE_INTEL:
         Architecture = "x86";
         break;
      }
      COLcout << "Native Architecture: " << Architecture << newline;
      COLcout << newline;
#endif
#endif


      CMDlineParser Parser;

      Parser.addFlagWithoutArgument("read", "Read the current Machine ID");
      Parser.addFlagWithoutArgument("update", "Update the Machine ID to synchronize");
      Parser.addFlagWithoutArgument("h5", "Use h5 mode");
#ifdef _DEBUG
      Parser.addFlagWithoutArgument("erase", "Erase the Machine ID and exit");
#endif
      Parser.parseArgs(argc, argv);
      if (Parser.parsingErrorsPresent(COLcout))
      {
         Parser.showUsage(COLcout);
         return EXIT_FAILURE;
      }

      bool Read = Parser.isFlagPresent("read");
      bool Write = Parser.isFlagPresent("update");
      bool UseHash5 = Parser.isFlagPresent("h5");
#ifdef _DEBUG
      bool Erase = Parser.isFlagPresent("erase");
#else
      bool Erase = false;
#endif

      // COLstring RgnMachineId = RGNmachineId();
      // don't compute that yet - we want to have a shot at the first computation,
      // to see how it's calculated from raw inputs, not just get the cached copy

      // COLcout << "Machine ID: " << RgnMachineId << newline;
      // COLcout << newline;
      COLstring HostName = IPlocalHostName();
      COLcout << "HostName: " << HostName << newline;
      COLcout << newline;

      COLdateTime DateTime = COLdateTime::currentTime();
      COLstring DateStr = DateTime.format("%Y-%m-%d");

      COLcout << "Date: " << DateStr << newline;
      COLcout << newline;


      if(Erase)
      {
         // Erase the secret cached machine id
         REGregistry Registry(HKEY_LOCAL_MACHINE);
         COLstring KeyPath = RGNsoftwareRegistryPath(CHMmachineIdSubPath);
         Registry.deleteSubKey(KeyPath);
         COLcout << "Machine ID has been erased." << newline;
         COLcout << newline;
         return EXIT_SUCCESS;
      }

      COLstring Log;
      COLostream LogStream(Log);

      COLstring Salt = DateStr+"_"+HostName;
      DBEencryptionKey Key = DBEcreateEncryptionKey(Salt);

      COLcout << "NOTE: When sending the output to support, please copy and paste it as text" << newline;
      COLcout << newline;

      COLstring MachineId;
      try
      {
         MachineId = MICmachineId(LogStream, Read, Write, UseHash5);
      }
      catch(const COLerror& Error)
      {
         // if it throws, make sure we still produce the encrypted log dump -
         // might reveal some underlying problems, e.g. specific errors with registry access
         COLcout << "Caught error while getting Machine ID:" << newline;
         COLcout << Error << newline;
         COLcout << newline;
         COLcout << DBEencryptString(Log, Key) << newline;
         COLcout << newline;
         throw;
      }

      COLcout << "Machine ID: " << MachineId << newline;
      COLcout << newline;

      COLcout << DBEencryptString(Log, Key) << newline;

      return EXIT_SUCCESS;
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
