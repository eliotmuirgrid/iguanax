// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCtooltipSet
//
// Description:
//
// Implementation
//
// Author: Tyler Brown
// Date:   Monday 13 May 2024 - 12:25PM
// ---------------------------------------------------------------------------

#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>
#include <CMPC/CMPCtooltipSet.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
COL_LOG_MODULE;

// /component/tooltip/set
void CMPCtooltipSet(const COLwebRequest& Request, CFGmap* pMap) {
   COL_FUNCTION(CMPCtooltipSet);
   const COLvar& Input = Request.Data;
   COL_VAR(Input.json(1));
   const COLstring Component        = COLvarRequiredString(Input, "component");
   const COLstring HtmlTooltipBlock = COLvarRequiredString(Input, "html");
   CFGconfig*      pConfig          = CFGmapNode(pMap, Component);
   if(!pConfig) { return COLrespondError(Request.Address, "No component exists with name or id " + Component); }
   pConfig->Status.HtmlTooltipBlock = HtmlTooltipBlock;
   COLrespondSuccess(Request.Address);
}