// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SERVconfig
//
// Description:
//
// Implementation
//
// Author: Tyler Brown
// Date:   Wednesday 23 August 2023 - 11:39AM
// ---------------------------------------------------------------------------

#include <SERV/SERVconfig.h>

#include <SERV/SERVdescription.h>
#include <SERV/SERVdisplayName.h>
#include <SERV/SERVworkingDir.h>

#include <SDM/SDMname.h>

#include <INI/INIdata.h>
#include <INI/INIsyntaxCheck.h>

#include <LINK/LINKconstants.h>

#include <POP/POPcolor.h>

#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void SERVsettingServiceConfig(const INIdata& Config){
   COL_FUNCTION(SERVsettingServiceConfig);
   COLstring WorkingDir  = INIdataKeyGet(Config, "server", "", "working_dir");
   COLstring Name        = INIdataKeyGet(Config, "server", "", "name");
   COLstring DisplayName = INIdataKeyGet(Config, "server", "", "display_name");
   COLstring Description = INIdataKeyGet(Config, "server", "", "description");
   COL_VAR4(WorkingDir, Name, DisplayName, Description);
   SERVsetWorkingDir (WorkingDir);
   SERVsetDisplayName(DisplayName);
   SERVsetDescription(Description);
   SDMsetServiceName(Name);
}

INIdata SERVexpectedDataFormat(){
   COL_FUNCTION(SERVexpectedDataFormat);
   INIdata Expected;
   INIdataKeySet(&Expected,"server", "", "working_dir",  "");
   INIdataKeySet(&Expected,"server", "", "name",         "");
   INIdataKeySet(&Expected,"server", "", "display_name", "");
   INIdataKeySet(&Expected,"server", "", "description",  "");
   return Expected;
}

COLstring SERVfileNameErrorMessage(const COLstring FileName){
   COL_FUNCTION(SERVfileNameErrorMessage);
   return "///// UNEXPECTED FORMAT IN FILE: " + FileName;
}

void SERVloadServiceConfiguration(COLstring* pFileName){
   COL_FUNCTION(SERVloadServiceConfiguration);
   COLstring Dir = FILexecutableDir();
   COLstring FileName = FILexecutableFileName(false) + ".ini";
   COLstring FilePath = Dir + FileName;
   COL_VAR(FilePath);
   if (!FILfileExists(FilePath)) {
      COL_TRC("No service configuration file found in: " + Dir );
      return;
   }
   COLcout << "Found service configuration file: '" << FilePath << "'" << newline;
   *pFileName = FilePath;
   INIdata Config;
   COLstring Data;
   FILreadFile(FilePath, &Data);
   INIparse(Data, &Config);
   INIdata Expected = SERVexpectedDataFormat();
   COLstring Output;
   if(!INIsyntaxCheck(Expected, Config, &Output)){
      COLcout << newline << SERVfileNameErrorMessage(FilePath) << newline << newline;
      COLcout << Output << newline;
      COLcout << "For more info: " << POPpop(LINKservConfig) << newline << newline;
      COLcout << "////////////////////////////////////////////////////////////////" << newline << newline;
   }
   SERVsettingServiceConfig(Config);
}
