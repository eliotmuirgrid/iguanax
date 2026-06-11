// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTserviceUninstall
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday 12 July 2023 - 03:04PM
// ---------------------------------------------------------------------------

#include <INST/INSTserviceUninstall.h>

#include <SDM/SDMmanage.h>
#include <SDM/SDMname.h>

#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void INSTserviceUninstall(const COLwebRequest& Request){
   COL_FUNCTION(INSTserviceUninstall);
   COLstring Error;
   bool Success = SDMserverRemove(SDMserviceName(), &Error);
   if (Success){
      COLrespondSuccess(Request.Address);
   } else {
      COLrespondError(Request.Address, Error);
   }
}