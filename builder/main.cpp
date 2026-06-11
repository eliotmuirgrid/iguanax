//---------------------------------------------------------------------------
// Copyright (C) 2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: main.cpp
//
// Description:
//
// Builder watcher utility for monitoring GIT repo to trigger automated builds
//
// Author: Eliot Muir
// Date:   Tue 17 Aug 2021 14:24:47 EDT
//---------------------------------------------------------------------------

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <BUILD/BUILDwatch.h>

#include <FIL/FILpathUtils.h>

#include <CMD/CMDlineParser.h>

int main(int argc, const char** argv){
   try {
      CMDlineParser LineParser;
      LineParser.setDescription("Build watcher - triggers builds when necessary.");
      LineParser.addFlagWithArgument("dir", "Directory of git archive to monitor.");
      LineParser.parseArgs(argc,argv);
      if (LineParser.parsingErrorsPresent(COLcout)){
         LineParser.showUsage(COLcout);
         return EXIT_FAILURE;
      }

      COLstring Dir = LineParser.flagArgument("dir");
      Dir = FILpathResolveEnvironment(Dir);
      COLcout << "Monitoring: " << Dir << newline;

      BUILDloop(Dir);

   } catch (COLerror& Error) {
      COLcerr << Error.description() << newline;
      return EXIT_FAILURE;
   } catch(...) {
      COLcerr << "Unhandled Exception" << newline;
      return EXIT_FAILURE;
   }
   return EXIT_SUCCESS;
}
