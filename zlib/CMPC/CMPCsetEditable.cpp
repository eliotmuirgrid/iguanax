// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCsetEditable
//
// Description: Implementation
//
//
// Author: Matthew Sobkowwski
// Date:   Tuesday August 8, 2023
// ---------------------------------------------------------------------------
#include <CFG/CFGconfigSave.h>
#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>
#include <CMPC/CMPCsetEditable.h>
#include <CMPC/CMPCutils.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <GINS/GINSinitialize.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
COL_LOG_MODULE;

// /component/set_editable
void CMPCsetEditable(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop) {
   COL_FUNCTION(CMPCsetEditable);
   COL_VAR(Request.Data.json(1));
   COLstring  Component  = COLvarRequiredString(Request.Data, "component");
   bool       SkipCommit = COLvarGetBool(Request.Data, "skip_commit", false);
   CFGconfig* pConfig    = CFGmapNode(pMap, Component);
   if(!pConfig) { return COLrespondError(Request.Address, Component + " does not exist"); }
   COLstring Error;
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, pConfig->CoreConfig.Guid, ROLid::EDIT_SCRIPT, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_SCRIPT));
   }
   pConfig->CoreConfig.Template = false;
   CMPCsaveConfig(Request.User, *pConfig, SkipCommit, pLoop, Request.Address);
}