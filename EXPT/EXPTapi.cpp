// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: EXPTapi
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Friday 14 November 2025 - 12:43PM
// ---------------------------------------------------------------------------
#include <EXPT/EXPTapi.h>
#include <EXPT/EXPTprojectFiles.h>
#include <EXPT/EXPTsampleData.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void EXPTapi(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap){
   COL_FUNCTION(EXPTapi);
   ApiMap.add("/export/dev/files",   &EXPTprojectFiles);
   ApiMap.add("/export/dev/samples", &EXPTsampleData);
}