//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNserver
//
// Description:
//
// TUNserver - Tunnel Server
//
// Author: Eliot Muir
// Date:   Fri 16 Jun 2023 12:47:15 EDT
//---------------------------------------------------------------------------

#include <SCK/SCKutils.h>

#include <CMD/CMDlineParser.h>

#include <stdlib.h>

#include <TUNS/TUNSserver.h>

#include <SDM/SDMmanage.h>
#include <INI/INIdata.h>
#include <PRO/PROutils.h>

#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static COLstring TUNtraceFileWithPid(const COLstring& BaseName){
   //COL_FUNCTION(TUNtraceFileWithPid);
   int Pid = PROpid();
   COLstring Result = FILparentDir(FILexpand(BaseName));
   FILaddPathSeparator(Result);
   Result += FILpathNameWithoutLastExt(BaseName) + "." + COLintToString(Pid) + FILpathExtLast(BaseName, true);
   return Result;
}

static void TUNtrace() {
   INIdata Config;
   COLstring Data, InitFilePath = "trace.ini";
   if (!FILfileExists(InitFilePath)) { return; }
   FILreadFile(InitFilePath, &Data);
   INIparse(Data, &Config);
   COLcout << "Found initialization file '" << FILworkingDir() << InitFilePath << "'." << newline;

   COLstring LogFile = TUNtraceFileWithPid(INIdataKeyGet(Config, "trace", "", "TUN_TRACE_FILE"));
   COLsetTraceLogFile(LogFile);
   COLcout << "Logging to " << LogFile << newline;
   COLtrace(INIdataKeyGet(Config, "trace", "", "TUN_TRACE"));
   return;
}

int main(int argc, const char* argv[]){
   FILinitExecutableName(argv[0]);
   SCKinitWinsock();
   try{
      CMDlineParser Parser;
      Parser.setDescription("iNTERFACEWARE Tunnel Server 1.0");
      Parser.addFlagWithoutArgument("install", "Install the server as a daemon");
      Parser.parseArgs(argc, argv);
      if (Parser.parsingErrorsPresent(COLcout)){
         Parser.showUsage(COLcout);
         return EXIT_FAILURE;
      }
      if (Parser.isFlagPresent("install")){
         COLstring Error;
         SDMserverInstall("tunnel", "Tunnel Server", "Secure server for IguanaX communication", FILexecutableDir() + FILexecutableFileName(), "root", "", true, &Error);
         if (!Error.is_null()) COLcout << Error;
         return EXIT_SUCCESS;
      }

      TUNtrace();
      TUNSserver();

   } catch (const COLerror& Error){
      COLcout << Error;
      return EXIT_FAILURE;
   }
   return EXIT_SUCCESS;
}
