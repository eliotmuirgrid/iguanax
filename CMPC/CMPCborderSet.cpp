// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCborderSet
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Friday 19 May 2023 - 1:00PM
// ---------------------------------------------------------------------------
#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>
#include <CMPC/CMPCborderSet.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
COL_LOG_MODULE;

// /component/border/set
void CMPCborderSet(const COLwebRequest& Request, CFGmap* pMap) {
   COL_FUNCTION(CMPCborderSet);
   const COLvar& Input = Request.Data;
   COL_VAR(Input.json(1));
   COLstring Component = COLvarRequiredString(Input, "component");
   COLstring Color     = COLvarRequiredString(Input, "color");
	CFGconfig* pConfig = CFGmapNode(pMap, Component);
	if(!pConfig) { return COLrespondError(Request.Address, "No component exists with name or id " + Component); }
	pConfig->Status.Border = Color;
	COLrespondSuccess(Request.Address);
}