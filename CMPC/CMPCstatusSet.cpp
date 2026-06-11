// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCstatusSet
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Friday 19 May 2023 - 10:00AM
// ---------------------------------------------------------------------------
#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>
#include <CMPC/CMPCstatusSet.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
COL_LOG_MODULE;

// /component/status/set
void CMPCstatusSet(const COLwebRequest& Request, CFGmap* pMap) {
   COL_FUNCTION(CMPCstatusSet);
   const COLvar& Input = Request.Data;
   COL_VAR(Input.json(1));
   const COLstring Component = COLvarRequiredString(Input, "component");
   const COLstring HtmlBlock = COLvarRequiredString(Input, "html");
   CFGconfig*      pConfig   = CFGmapNode(pMap, Component);
   if(!pConfig) { return COLrespondError(Request.Address, "No component exists with name or id " + Component); }
   pConfig->Status.HtmlBlock = HtmlBlock;
   COLrespondSuccess(Request.Address);
}