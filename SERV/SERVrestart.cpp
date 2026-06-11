// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SERVrestart
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Friday 01 September 2023 - 07:59AM
// ---------------------------------------------------------------------------

#include <SERV/SERVrestart.h>
#include <SERV/SERVuninstall.h>
#include <SERV/SERVutils.h>

#include <stdlib.h>

#include <SDM/SDMmanage.h>
#include <SDM/SDMname.h>

#include <IWEB/IWEBiguanaUrl.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

#ifndef __APPLE__
int SERVrestart(){
   COL_FUNCTION(SERVrestart);
   bool Exists;
   COLstring Error;
   if (!SDMserverExists(SDMserviceName(), &Exists, &Error)) {
      COLcout << "While detecting " << SDMserviceName() << " service:" << Error << newline;
      COL_VAR2(Exists, Error);
      return EXIT_FAILURE;
   }
   COL_VAR2(Exists, Error);
   if (!Exists){
      COLcout << "Service " << SDMserviceName() << " does not exist." << newline;
      return EXIT_FAILURE;
   }
   if(!SERVisSudo()) { return EXIT_FAILURE; }
   if (!SERVstopServerWithRetry()){
      COLcout << "Failed to stop service." << newline;
      return EXIT_FAILURE;
   }
   bool Success = SDMserverStart(SDMserviceName(), &Error);
   if (!Success){
      COLcout << "Failed to restart server: " << Error << newline;
      return EXIT_FAILURE;
   }
   COLcout << "Restarted server." << newline;
   IWEBoutputListeningAndHelp();
   return EXIT_SUCCESS;
   // TODO - should we consider logic to test if the server is running?
}
#endif