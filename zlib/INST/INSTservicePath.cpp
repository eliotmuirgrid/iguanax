// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTservicePath
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Sunday 16 July 2023 - 10:13AM
// ---------------------------------------------------------------------------

#include <INST/INSTservicePath.h>

#include <SDM/SDMmanage.h>
#include <SDM/SDMname.h>

#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// /install/service/path
void INSTservicePath(const COLwebRequest& Request){
   COL_FUNCTION(INSTservicePath);
   COLvar Result;
   COLstring FileName, Error;
   if (SDMserverExe(SDMserviceName(), &FileName, &Error)) {
      Result["filename"] = FileName;
      COLrespondSuccess(Request.Address, Result);
   } else {
      COLrespondError(Request.Address, Error);
   }
}