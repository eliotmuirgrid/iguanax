// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: main.cpp
//
//  Description:
//
//  Command line utility to generate repetitive boilerplate for C++ header
//  and cpp files.
//
//  Author: Eliot Muir
//  Date:   Sat 17 Sep 2022 15:13:46 EDT
// ---------------------------------------------------------------------------

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <CMD/CMDlineParser.h>
#include <PRO/PROexecute.h>

#include <stdlib.h>

#include "generate.h"
#include "generateTest.h"

COLstring getDefaultAuthor() {
   COLstring OutErr;
   int Result = PROexecuteString("", "git config user.name", &OutErr, &OutErr);
   return Result == 0 ? COLstripWhiteSpace(OutErr) : "Eliot Muir";
}

int main(int argc, const char** argv){
   CMDlineParser Parser;

   COLcout << "C++ Generator 1.0" << newline;
   Parser.addFlagWithArgument("file",   "Name of file to generate");
   Parser.addFlagWithArgument("author", "Author of code");
   Parser.addFlagWithArgument("test",   "Generate unit test file.");
   try{
      Parser.parseArgs(argc, argv);
      if (Parser.parsingErrorsPresent(COLcout)){
         Parser.showUsage(COLcout);
         return EXIT_FAILURE;
      }
      COLstring Author = getDefaultAuthor();
      if (Parser.isFlagPresent("author")){
         Author = Parser.flagArgument("author");
      }
      if (Parser.isFlagPresent("file")){
         CPPgenerateCode(Parser.flagArgument("file"), Author);
         return EXIT_SUCCESS;
      } else if (Parser.isFlagPresent("test")){
         CPPgenerateTestFiles(Parser.flagArgument("test"), Author);
         return EXIT_SUCCESS;
      }
      COLcout << "Please specify a --file <filename> or --test <filename>" << newline;

   } catch(const COLerror& Error){
      COLcout << Error.description() <<  newline;
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}
