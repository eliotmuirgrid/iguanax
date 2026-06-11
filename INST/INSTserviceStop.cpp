// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTserviceStop
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday 12 July 2023 - 03:04PM
// ---------------------------------------------------------------------------

#include <INST/INSTserviceStop.h>

#include <SDM/SDMmanage.h>
#include <SDM/SDMname.h>

#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void INSTserviceStop(const COLwebRequest& Request){
   COL_FUNCTION(INSTserviceStop);
   COLstring Error;
   bool Success = true;
   #ifndef __APPLE__
   Success = SDMserverStop(SDMserviceName(), &Error);
   #endif
   if (Success){
      COLrespondSuccess(Request.Address);
   } else {
      COLrespondError(Request.Address, Error);
   }
}