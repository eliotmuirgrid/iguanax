//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: INSTaccountExists.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   6/22/23 12:04 PM
//  ---------------------------------------------------------------------------
#include <INST/INSTaccountExists.h>

#include <SDM/SDMmanage.h>
#include <SDM/SDMname.h>

#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// /install/account_exists
void INSTaccountExists(const COLwebRequest& Request) {
   COL_FUNCTION(INSTaccountExists);
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