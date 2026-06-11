// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTserviceExists
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday 12 July 2023 - 03:04PM
// ---------------------------------------------------------------------------

#include <INST/INSTserviceExists.h>

#include <SDM/SDMmanage.h>
#include <SDM/SDMname.h>

#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void INSTserviceExists(const COLwebRequest& Request){
   COL_FUNCTION(INSTserviceExists);
   bool Exists;
   COLstring Error;
   bool Success = SDMserverExists(SDMserviceName(), &Exists, &Error);
   if (Success) {
      COLvar Result;
      Result["exists"] = Exists;
      COLrespondSuccess(Request.Address, Result);
   } else {
      COLrespondError(Request.Address, Error);
   }
}