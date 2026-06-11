// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTurl
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 12 July 2023 - 12:43PM
// ---------------------------------------------------------------------------

#include <INST/INSTurl.h>

#include <IWEB/IWEBiguanaUrl.h>

#include <COL/COLweb.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void INSTurl(const COLwebRequest& Request){
   COL_FUNCTION(INSTurl);
   COLvar Result;
   Result["url"] = IWEBiguanaUrl();
   COL_VAR(Result.json(1));
   COLrespondSuccess(Request.Address, Result);
}