//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Tool to generate GIT information file - works cross platform without
// stinky shell script
//
// Author: Eliot Muir
// Date:   Wed 21 Dec 2022 16:05:15 EST
//---------------------------------------------------------------------------

#include <PRO/PROexecute.h>
#include <FIL/FILutils.h>

#include <CMD/CMDlineParser.h>

#include <stdlib.h>

#include <COL/COLostream.h>
#include <COL/COLstring.h>
#include <COL/COLsinkString.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring GITcommand(const COLstring& Command){
   COL_FUNCTION(GITcommand);
   int ExitCode;
   COLstring Result;
   COLstring Err;
   ExitCode = PROexecuteString("", "git " + Command, &Result, &Err);
   Result.stripAll('\n');
   Result.stripAll('\r');
   return Result;
}

void writeFileAndLog(const COLstring& FileName, const COLstring& Content){
   FILwriteFile(FileName, Content);
   COLcout << "Wrote " << FileName << newline << Content;
}

int main(int argc, const char** argv){
   CMDlineParser Parser;
   Parser.parseArgs(argc, argv);

   COLstring OutFile = "../DBDVER/DBDVERgitInfo.cpp";
   COLstring Out;
   COLsinkString Sink(Out);
   COLostream Stream(Sink);
   Stream << "// Automatically generated file - do not edit by hand." << newline
          << "const char* DBDVER_BRANCH_NAME= \"" << GITcommand("rev-parse --abbrev-ref HEAD") << "\";" << newline
          << "const char* DBDVER_TIME_STAMP = \"" << GITcommand("show -s --format=%ct HEAD")   << "\";" << newline
          << "const char* DBDVER_COMMIT_ID  = \"" << GITcommand("rev-parse HEAD")              << "\";" << newline
          << "const char* DBDVER_COMMIT_MSG = \"" << GITcommand("log -1 --pretty=%B")          << "\";" << newline;

   if (!FILfileExists(OutFile)){
      writeFileAndLog(OutFile, Out);
      return EXIT_SUCCESS;
   }
   COLstring Content;
   FILreadFile(OutFile, &Content);
   if (Content != Out){
      COLcout << "Version information changed:" << newline;
      writeFileAndLog(OutFile, Out);
   } 
   return EXIT_SUCCESS;
}
