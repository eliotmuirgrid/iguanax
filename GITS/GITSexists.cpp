//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GITSexists.cpp
//
//  Description:
//
//  Author: Matthew Sobkowski
//  Date:   5/2/23 3:12 PM
//  ---------------------------------------------------------------------------
#include "GITSexists.h"

#include <PRO/PROwhich.h>

#include <stdlib.h>

#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// /git/exists
void GITSexists(const COLwebRequest& Request) {
   COL_FUNCTION(GITSexists);
   COLstring GitPath;
   bool Exists = PROwhich(&GitPath, "git");
   COLvar Result;
   Result["exists"] = Exists;
   if (getenv("GITS_FAKE_NOT_PRESENT")){
      COL_ERR("GITS_FAKE_NOT_PRESENT is set so simulating GIT not present.");
      Result["exists"] = false;
   }
   COLrespondSuccess(Request.Address, Result);
}