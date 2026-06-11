// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTstop
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday 12 July 2023 - 08:13AM
// ---------------------------------------------------------------------------

#include <INST/INSTstop.h>

#include <SCK/SCKloop.h>

#include <SDM/SDMserver.h>
#include <SDM/SDMshutdown.h>
#include <SDM/SDMmanage.h>
#include <SDM/SDMname.h>

#include <COL/COLweb.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static bool s_StartAfterShutdown = false;

void INSTstartServiceAfterShutdown(){
   COL_FUNCTION(INSTstartServiceAfterShutdown);
   if (s_StartAfterShutdown){
      COLcout << "Starting service" << newline;
      COLstring Error;
      #ifndef __APPLE__
      if (!SDMserverStart(SDMserviceName(),&Error)){
         COLcout << Error << newline;
      }
      #endif
   }
}

void INSTstop(SDMserver* pServer){
   COL_FUNCTION(INSTstop);
   s_StartAfterShutdown = true;
   SDMshutdown();
}

void INSTstop(const COLwebRequest& Request, SDMserver* pServer, SCKloop* pLoop){
   COL_FUNCTION(INSTstop);
   pLoop->addTimer(COLnewClosure0(&INSTstop, pServer), 0);
   COLrespondSuccess(Request.Address);
}
