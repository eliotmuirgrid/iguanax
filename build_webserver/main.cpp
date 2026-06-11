//---------------------------------------------------------------------------
// Copyright (C) 2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: main.cpp
//
// Description:
//
// Builder webserver for serving up the log files of the build.
//
// Author: Eliot Muir
// Date:   Tue 17 Aug 2021 14:24:47 EDT
//---------------------------------------------------------------------------

#include <COL/COLthreadPool.h>
#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


#include <FIL/FILpathUtils.h>

#include <CMD/CMDlineParser.h>

#include <WEB/WEBserver.h>
#include <WEB/WEBfileDirHandler.h>

#include <SCK/SCKloop.h>


int main(int argc, const char** argv){
   try {
      
      CMDlineParser LineParser;
      LineParser.setDescription("Build watcher - triggers builds when necessary.");
      LineParser.addFlagWithArgument("dir", "Directory of web files to serveRequest.");
      LineParser.addFlagWithArgument("port", "Port to listen on.");
      LineParser.parseArgs(argc,argv);
      if (LineParser.parsingErrorsPresent(COLcout)){
         LineParser.showUsage(COLcout);
         return EXIT_FAILURE;
      }

      int Port = 8080;
      COL_VAR(Port);
      if (LineParser.isFlagPresent("port")){
         Port = atoi(LineParser.flagArgument("port").c_str());
         COL_VAR(Port);
      }
      COLstring Dir = ".";
      if (LineParser.isFlagPresent("dir")){
         Dir = LineParser.flagArgument("dir");
      }
      Dir = FILpathResolveEnvironment(Dir);
      COLcout << "Serving: " << Dir << " on port " << Port << newline;

      COLthreadPool Pool;
      Pool.start();
      SCKloop Loop(&Pool);
      WEBserver Server(Loop);
      WEBfileDirHandler Handler(&Loop, Dir);
      Server.registerHandler("/", WEBcreateHandler(&Handler, &WEBfileDirHandler::serve) );
      Server.start(Port);
      Loop.start();
      


   } catch (COLerror& Error) {
      COLcerr << Error.description() << newline;
      return EXIT_FAILURE;
   } catch(...) {
      COLcerr << "Unhandled Exception" << newline;
      return EXIT_FAILURE;
   }
   return EXIT_SUCCESS;
}
