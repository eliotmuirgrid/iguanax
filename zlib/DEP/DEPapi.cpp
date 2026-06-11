// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DEPapi
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Wednesday 16 April 2025 - 06:50AM
// ---------------------------------------------------------------------------

#include <DEP/DEPapi.h>
#include <DEP/DEPlist.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void DEPapiWorker(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap) {
   COL_FUNCTION(DEPapiWorker);
   ApiMap.add("/deprecated/lua", &DEPlua);
}
