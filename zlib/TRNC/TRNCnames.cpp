// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNCnames
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Monday 24 April 2023 - 01:52PM
// ---------------------------------------------------------------------------
#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <TRNC/TRNCnames.h>
COL_LOG_MODULE;

// /component/names
void TRNCnames(const COLwebRequest& Request, CFGmap* pMap) {
   COL_FUNCTION(TRNCnames);
   COLvar Output;
   CFGmapNames(pMap, &Output);
   COL_VAR(Output);
   COLrespondSuccess(Request.Address, Output);
}

