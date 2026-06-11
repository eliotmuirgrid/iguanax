//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: main.cpp
//
// Description:
//
// An example command line app
//
// Author: Eliot Muir
//---------------------------------------------------------------------------

#include <COL/COLostream.h>
#include <COL/COLerror.h>
#include <stdlib.h>

#include <CMD/CMDlineParser.h>

#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

int main(int argc, const char** argv) {
   try{
      CMDlineParser Parser;
      Parser.addFlagWithoutArgument("run", "Run this thing");
      Parser.addFlagWithArgument("file", "name", "Write data to this file");
      Parser.parseArgs(argc, argv);

      COL_TRC("Parsed " << argc << " arguments");
      COL_DBG("This is really detailed tracing.");
      COL_VAR(argc);
      if (Parser.parsingErrorsPresent(COLcout)){
         Parser.showUsage(COLcout);
         return EXIT_FAILURE;
      }
      
      if (Parser.isFlagPresent("run")){
         COLcout << "Okay run for the hills!" << newline;
      }

      COLstring File = "default";
      if (Parser.isFlagPresent("file")){
         Parser.flagArgument("file", File);
      }
      COLcout << "File:" << File << newline;

      COLstring Path = FILpathAppend(FILworkingDir(), File);
      COLcout << "Writing file:" << Path << newline;
      FILwriteFile(Path, "Done");
      COLcout << "Done!" << newline; 

      return EXIT_SUCCESS;
   } catch (COLerror& Error) {
      COLcerr << Error.description() << newline;
      return EXIT_FAILURE;
   } catch(...) {
      COLcerr << "Unhandled Exception" << newline;
      return EXIT_FAILURE;
   }
   return EXIT_SUCCESS;
}
