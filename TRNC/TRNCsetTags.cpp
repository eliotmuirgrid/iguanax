// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNCsetTags
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Friday 16 February 2024 - 11:26AM
// ---------------------------------------------------------------------------
#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <HASH/HASHutils.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKloop.h>
#include <TRNC/TRNCsaveConfig.h>
#include <TRNC/TRNCsetTags.h>

COL_LOG_MODULE;

void TRNCsetTagsImpl(const COLstring& Component, const COLvar& Value, CFGmap* pMap, const COLstring& Username, SCKloop* pLoop, const COLaddress& Address) {
   COL_FUNCTION(TRNCsetTagsImpl);
   COLstring Error;
   CFGconfig* pConfig = CFGmapNode(pMap, Component);
   COLarray<COLstring> NewTags;
   if(!HASHvarToArr(Value, &NewTags)) { return COLrespondError(Address, HASHparseError()); }
   pConfig->CoreConfig.Tags = NewTags;
   TRNCsaveConfig(Username, *pConfig, pLoop, Address);
}

// /component/tags/set
void TRNCsetTags(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop) {
   COL_FUNCTION(TRNCsetTags);
   COL_VAR(Request.Data.json(1));
   const COLstring Component = COLvarRequiredString(Request.Data, "component");
   const COLvar    Value     = COLvarRequiredObject(Request.Data, "value");
   COLstring       Err;
   CFGconfig* pConfig = CFGmapNode(pMap, Component);
   if (!pConfig){ return COLrespondError(Request.Address, "No component exists with name or id " + Component); }
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, Component, ROLid::EDIT_TAGS, &Err)) {
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_TAGS));
   }
   TRNCsetTagsImpl(Component, Value, pMap, Request.User, pLoop, Request.Address);
}
