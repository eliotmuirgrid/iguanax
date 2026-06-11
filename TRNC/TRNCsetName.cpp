//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: TRNCsetName
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
#include <TRNC/TRNCsetName.h>
#include <USR/USRuser.h>
COL_LOG_MODULE;

// TODO component name size check should not include escape characters in the check see IX-1721
static void TRNCsetNameImpl(const COLstring& Guid, const COLstring& Value, const COLaddress& Address, CFGmap* pMap, const COLstring& Username, SCKloop* pLoop) {
   COL_FUNCTION(TRNCsetNameImpl);
    CFGconfig* pConfig = CFGmapNode(pMap, Guid);
   if (Value.is_null())                           { return COLrespondError(Address, "A component name cannot be an empty string."); }
   if (Value.size() > 60)                         { return COLrespondError(Address, "A component name cannot be greater than 60 characters."); }
   if (CFGmapNameExists(pMap, Guid, Value)) { return COLrespondError(Address, "A component with the same name already exists."); }
   pConfig->CoreConfig.Name = Value;
   return TRNCsaveConfig(Username, *pConfig, pLoop, Address);
}

// /component/name/set
void TRNCsetName(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop) {
   COL_FUNCTION(TRNCsetName);
   COL_VAR(Request.Data.json(1));
   const COLstring Component = COLvarRequiredString(Request.Data, "component");
   const COLstring Value     = COLvarRequiredString(Request.Data, "value");
   COLstring       Err;
   CFGconfig* pConfig = CFGmapNode(pMap, Component);
   if (!pConfig){ return COLrespondError(Request.Address, "No component exists with name or id " + Component); }
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, Component, ROLid::EDIT_NAME, &Err)) {
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_NAME));
   }
   TRNCsetNameImpl(Component, Value, Request.Address, pMap, Request.User, pLoop);
}
