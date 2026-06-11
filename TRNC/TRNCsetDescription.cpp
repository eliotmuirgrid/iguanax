//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: TRNCsetDescription
//
// Author: Matthew Sobkowski
//
// Implementation
//---------------------------------------------------------------------------
#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <PRM/PRMcallbacks.h>
#include <SCK/SCKloop.h>
#include <TRNC/TRNCsaveConfig.h>
#include <TRNC/TRNCsetDescription.h>
COL_LOG_MODULE;

static void TRNCsetDescriptionImpl(const COLstring& Guid, const COLstring& Value, CFGconfig* pConfig,
                                   const COLstring& Username, SCKloop* pLoop, const COLaddress& Address) {
   COL_FUNCTION(TRNCsetDescriptionImpl);
   COLstring Error;
   pConfig->CoreConfig.Description = Value;
   TRNCsaveConfig(Username, *pConfig, pLoop, Address);
}

// /component/description/set
void TRNCsetDescription(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop) {
   COL_FUNCTION(TRNCsetDescription);
   COL_VAR(Request.Data.json(1));
   const COLstring Component = COLvarRequiredString(Request.Data, "component");
   const COLstring Value     = COLvarRequiredString(Request.Data, "value");
   COLstring       Err;
   CFGconfig* pConfig = CFGmapNode(pMap, Component);
   if(!pConfig) { return COLrespondError(Request.Address, "No component exists with name or id " + Component); }
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, Component, ROLid::EDIT_DESCRIPTION, &Err)) {
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_DESCRIPTION));
   }
   TRNCsetDescriptionImpl(Component, Value, pConfig, Request.User, pLoop, Request.Address);
}
