// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: main.cpp
//
//  Description: Command line utility to corrupt log data
//
//  Author: Vismay Shah
//  Date:   Friday September 15, 2023
// ---------------------------------------------------------------------------
#include <cstdlib>
#include <cstdio>

#include <FIL/FILpathUtils.h>
#include <FIL/FILdirEnumerator.h>

#include <SDB/SDBdir.h>

#include <CMD/CMDlineParser.h>
#include <DIR/DIRpath.h>
#include <SCK/SCKutils.h>

#include <COL/COLvar.h>
#include <COL/COLarray.h>
#include <COL/COLmath.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring serverDirectory(){
   COL_FUNCTION(serverDirectory);
   FILdirEnumerator DirEnumerator(DIRconfigDirDefault() + "*", true, true);
   COLstring Dir;
   while (DirEnumerator.getFile(Dir)){
      if ((Dir[0] != '_')){
         return DIRconfigDirDefault() + Dir + FIL_DIR_SEPARATOR;
      } else {
         COL_TRC("Ignoring " << Dir);
      }
   }
   return "";
}

bool checkLogsExist(const COLstring& ComponentLogDir){
   COL_FUNCTION(checkLogsExist);
   if (!FILfileExists(ComponentLogDir)) {
      COLcout << ComponentLogDir << " does not exist." << newline;
      return false;
   }
   if (!FILisDirectory(ComponentLogDir)) {
      COLcout << ComponentLogDir << " is not a directory." << newline;
      return false;
   }
   return true;
}


static void corruptLogFile(const COLstring& File, int MaxEdits){
   COL_FUNCTION(corruptLogFile);
   COLstring Content;
   FILreadFile(File, &Content);
   int Size = Content.size();
   COLarray<int> EditLocations;
   // Generate some random indices - could have duplicates but that should be ok
   for (int i = 0; i < MaxEdits; ++i) {
      int RandomIndex = COLmath::randomNumber(Size);
      EditLocations.push_back(RandomIndex);
   }
   COLvar Indices;
   for (int i = 0; i < EditLocations.size(); i++) {
      Indices.push_back(EditLocations[i]);
      Content[EditLocations[i]] += 32;       // modify the byte
      // Content.remove(i, 1); // A single character deletion is enough to corrupt the marker, crc, content or size.
   }
   FILwriteFile(File, Content);
   // COLcout << File << newline;
   // COLcout << Indices.json() << newline;
}

static void corruptLogs(const COLstring& LogsDir, const COLstring& Component, int Frequency, int MaxEdits){
   COL_FUNCTION(corruptLogs);
   COLstring NextFile = SDBfindOldest(LogsDir);
   int FileCount = 0;
   COLarray<COLstring> CorruptedFiles;
   COL_VAR(NextFile);
   COLmath::resetRandomGenerator();
   do {
      FileCount++;
      if (FileCount % Frequency == 0){
         CorruptedFiles.push_back(NextFile);
         corruptLogFile(NextFile, MaxEdits);
      }
      NextFile = SDBfindNextLogFile(NextFile, Component);
   } while (NextFile != "");
   COLcout << "Corrupted the following files: " << newline;
   for (int i = 0; i < CorruptedFiles.size(); i++){
      COLcout << CorruptedFiles[i] << newline;
   }
}

int main(int argc, const char** argv) {
   SCKinitWinsock();
   CMDlineParser Parser;
   COLcout << "Log Corruptor Tool 1.0." << newline;
   COLcout << "This tool corrupts log messages for a specified component." << newline;
   // required arg
   Parser.addFlagWithArgument("component", "The component whose logs will be corrupted");
   Parser.addFlagWithArgument("frequency", "Will corrupt logs every <frequency> log files");
   Parser.addFlagWithArgument("edits",     "The number of modifications to make per log file, default 100.");

   try {
      Parser.parseArgs(argc, argv);
      if(Parser.parsingErrorsPresent(COLcout)) {
         Parser.showUsage(COLcout);
         return EXIT_FAILURE;
      }
      // Command line arguments
      COLstring Component;
      if (!Parser.isFlagPresent("component")){
         COLcout << "must use --component to specify the component guid" << newline;
         return EXIT_FAILURE;
      } else { Component = Parser.flagArgument("component"); }
      int Frequency;
      if (!Parser.isFlagPresent("frequency")){ Frequency = 10; }
      else                                   { Frequency = atoi(Parser.flagArgument("frequency").c_str()); }
      int Edits;
      if (!Parser.isFlagPresent("edits")){ Edits = 100; }
      else                               { Edits = atoi(Parser.flagArgument("edits").c_str()); }
      FILsetWorkingDir(serverDirectory());
      COLstring LogDirectory, Error;
      if (!SDBsetUpLogDirectory(&LogDirectory, false, &Error) && !Error.is_null()) {
         COLcout << "Failed to determine custom log directory: " << Error << newline;
      }
      COLcout << "Log Dir: " << FILpathResolve(LogDirectory) << newline;
      COLcout << "Logs root directory: " << LogDirectory << newline;
      COL_VAR2(Component, LogDirectory);
      // implementation
      COLstring ComponentLogDir = FILpathAppend(LogDirectory, Component);
      if (!checkLogsExist(ComponentLogDir)) {
         return EXIT_FAILURE;
      }
      COLcout << "Corrupting logs... " << newline;
      corruptLogs(ComponentLogDir, Component, Frequency, Edits);
      COLcout << "Finished." << newline;
   } catch(const COLerror& Error) {
      COLcout << Error.description() << newline;
      return EXIT_FAILURE;
   }
   return EXIT_SUCCESS;
}
