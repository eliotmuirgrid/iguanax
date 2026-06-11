//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: DASHcomponentDetail.cpp
//
//  Description: Implementation
//
// Author: Matthew Nitsopoulos
// Date:   Wednesday August 24 2022
//  ---------------------------------------------------------------------------
#include <CFG/CFGconfigSave.h>
#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <SDB/SDBcapi.h>
#include <DASH/DASHcomponentDetail.h>
#include <DASH/DASHqueuePosition.h>
#include <DIR/DIRpath.h>
COL_LOG_MODULE;

static COLvar DASHcardCopyComponent(const CFGconfig& In) {
   COL_FUNCTION(DASHcardCopyComponent);
   COLvar Data;
   CFGcoreToVar(In.CoreConfig, &Data);
   Data["initialized"] = In.m_Initialized;
   Data["error_count"] = In.Status.ErrorCount;
   Data["html_block"]  = In.Status.HtmlBlock;
   Data["light"]       = CFGoutputLight(In.Status.Light);
   Data["queue_count"]  = SDBIgetQueueCount(In.CoreConfig.Guid);
   Data["source"]      = DIRcomponentDevDir(In.CoreConfig.Guid);
   COL_VAR(Data.json(1));
   return Data;
}

// /component/detail
void DASHcomponentDetail(const COLwebRequest& Request, CFGmap* pMap) {
   COL_FUNCTION(DASHcomponentDetail);
   auto pComponent = CFGmapNode(pMap, COLvarRequiredString(Request.Data, "component"));
   if(!pComponent) {
      COL_TRC("Component does not exist");
      return COLrespondError(Request.Address, "No component exists with name or id " + Request.Data["component"].asString());
   }
   COLrespondSuccess(Request.Address, DASHcardCopyComponent(*pComponent));
}
