// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDtrace
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 23 August 2023 - 10:27AM
// ---------------------------------------------------------------------------

#include <DBD/DBDtrace.h>

#include <INI/INIdata.h>

#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>

#include <PRO/PROutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring DBDtraceFileWithPid(const COLstring& BaseName){
   //COL_FUNCTION(DBDtraceFileWithPid);
   int Pid = PROpid();
   //COL_VAR(BaseName, Pid);
   COLstring Result = FILparentDir(FILexpand(BaseName));
   FILaddPathSeparator(Result);
   Result += FILpathNameWithoutLastExt(BaseName) + "." + COLintToString(Pid) + FILpathExtLast(BaseName, true);
   //COL_VAR(Result);
   return Result;
}

// TODO
// One could add syntax checking from INIsyntaxCheck
// Also support could be added for debug and info level tracing
// And whether to turn file log flushing on - it isn't currently
// See IX-1949

void DBDtrace() {
   INIdata Config;
   COLstring Data, InitFilePath = "trace.ini";
   if (!FILfileExists(InitFilePath)) { 
      //For debugging!
      //COLcout << "No trace file found in " << FILworkingDir() << newline;
      return; 
   }
   FILreadFile(InitFilePath, &Data);
   INIparse(Data, &Config);
   COLcout << "Found initialization file '" << FILworkingDir() << InitFilePath << "'." << newline;

   COLstring LogFile = DBDtraceFileWithPid(INIdataKeyGet(Config, "trace", "", "CHM_TRACE_FILE")); 
   COLsetTraceLogFile(LogFile);
   COLcout << "Logging to " << LogFile << newline;
   COLtrace(          INIdataKeyGet(Config, "trace", "", "CHM_TRACE"));
   return;
}