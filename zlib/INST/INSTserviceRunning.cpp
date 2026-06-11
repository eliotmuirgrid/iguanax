// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTserviceRunning
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday 12 July 2023 - 03:04PM
// ---------------------------------------------------------------------------

#include <INST/INSTserviceRunning.h>

#include <SDM/SDMmanage.h>
#include <SDM/SDMname.h>

#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void INSTserviceRunning(const COLwebRequest& Request){
   COL_FUNCTION(INSTserviceRunning);
   bool Running;
   COLstring Error;
   bool Success = SDMserverRunning(SDMserviceName(), &Running, &Error);
   if (Success) {
      COLvar Result;
      Result["running"] = Running;
      COLrespondSuccess(Request.Address, Result);
   } else {
      COLrespondError(Request.Address, Error);
   }
}
