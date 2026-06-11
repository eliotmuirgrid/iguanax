//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDMserver
//
// Description:
//
// An example of using the SDM library to set up a simple server.
//
// Depending on the command line flags, the code does the following:
//       1. installs the server
//       2. uninstalls the server
//       3. run the server as a normal process
//       4. Run as a service under the service control manager.
//
// Author: Filip Petr ?
// Date:   Thurs 07/15/2004 ?
//---------------------------------------------------------------------------
#include "SDMexamplePrecomp.h"
#pragma hdrstop

#include <CMD/CMDlineParser.h>

#include <COL/COLerror.h>
#include <COL/COLref.h>

#include <SDM/SDMservice.h>
#include <SDM/SDMserviceManagerWindows.h>
#include <SDM/SDMinstall.h>

#include "SDMexampleServer.h"

#include <stdlib.h>

#include <FIL/FILutils.h>

int main(int argc, const char* argv[]){
   try {      
      FILinitExecutableName(argv[0]);
      CMDlineParser LineParser;
      LineParser.setDescription("Example Service/Daemon");
 
      COLstring ServiceName = "FooKey"; 
      COLstring ServiceDisplayName = "Foo Example Service"; 
      COLstring ServiceDescription = "This is the Foo Example Service Description String"; 
      COLstring ServiceUser = "system";
      COLstring Password = "";
     
      SDMexampleServer Server; 
      SDMserver Svc(ServiceName);
      Svc.setOnRun(COLnewClosure0(&Server, &SDMexampleServer::onRun));
      Svc.setOnStop(COLnewClosure0(&Server, &SDMexampleServer::onStopRequest));
    
      LineParser.parseArgs(argc, argv);
#ifdef _WIN32  // Service install/uninstall logic
      if( LineParser.flagPresent("install","Install the service.")){
         SDMinstall(ServiceName, ServiceDisplayName, ServiceDescription, FILgetExecutableFileName(), ServiceUser, Password, true);
         return EXIT_SUCCESS;
      }

      if ( LineParser.flagPresent("info", "Information on the service.")){
         SDMserviceManager Manager;
         SDMservice& Service = Manager.service(ServiceName);
         COLcout << newline << 
                    "Name: " << Service.name() << newline <<
                    "Display Name: " << Service.displayName() << newline <<
                    "Executable: " << Service.executableFile() << newline;
         return EXIT_SUCCESS;
      }
      
      if(LineParser.flagPresent("disable", "Disable the service.")){
         COLcout << "Disabling service " << ServiceName << newline;
         SDMserviceManager ServiceManager;
         ServiceManager.service(ServiceName).disableService();
         return EXIT_SUCCESS;
      }

      if( LineParser.flagPresent("uninstall","Uninstall the service."))
      {
         SDMuninstall(ServiceName);
         return EXIT_SUCCESS;
      }
#endif //  _WIN32 Service install/uninstall logic finished.
      if (LineParser.flagPresent("run", "Run server as a normal process.")){
         Svc.runServerAsNormalProcess();
         return EXIT_SUCCESS;
      }

      if (LineParser.unknownFlagPresent(COLcout)){
         LineParser.showUsage(COLcout);
         return EXIT_FAILURE;
      } else {
         // run server as service or daemon
         if (!Svc.runServer()){  
            COLcerr << "Error:  If you wish to run this program like a normal process please use --run:" << newline;
            LineParser.showUsage(COLcerr);
            return EXIT_FAILURE;
         }
      }
   } catch (COLerror& Error) {
      COLcerr << Error << newline;           
      return EXIT_FAILURE;
   } catch (...) {
      COLcerr << "Unhandled exception in main." << newline;  
      return EXIT_FAILURE;
   }
   return EXIT_SUCCESS;
}

