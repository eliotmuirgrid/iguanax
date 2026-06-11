// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDMrestart
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Monday 28 August 2023 - 02:55PM
// ---------------------------------------------------------------------------
#include <SDM/SDMname.h>
#include <SDM/SDMmanage.h>
#include <SDM/SDMrestart.h>
#include <SDM/SDMshutdown.h>

#include <PRO/PROlaunch.h>
#include <APP/APPmain.h>

#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static bool s_SDMrestart = false;

void SDMrestartIfRequested(){
   COL_FUNCTION(SDMrestartIfRequested);
   if (false == s_SDMrestart) { return; }
#ifdef __APPLE__
   COL_TRC("Let the service restart iguana");
#else
   COLstring Error;
   bool Running = false;
   bool Success = SDMserverRunning(SDMserviceName(), &Running, &Error);
   if (!Success){
      COLcout << "Error: " << Error << " while checking if the server was running.\n";
      Running = false;
   }
   if (!Running){
      COLcout << "Start the server.\n";
      Success = SDMserverStart(SDMserviceName(),&Error);
      if (!Success){
         COLcout << "Error: " << Error << " while starting server.\n";
      }
   }
#endif
}

void SDMrequestRestart(){
   COL_FUNCTION(SDMrequestRestart);
   s_SDMrestart = true;
#ifdef __APPLE__
   APPkillMacController();
#endif
   SDMshutdown();
}

