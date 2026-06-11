//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: ROLtest.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   28/02/24 1:50 PM
//  ---------------------------------------------------------------------------
#include <ROL/ROLtest.h>
#include <ROL/ROLrole.h>

#include <ROL/ROLid.h>

#include <COL/COLvarUtils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

bool ROLtestImpl(const COLarray<COLstring>& Roles, ROLmap* pRoleMap) {
   COL_FUNCTION(ROLtestImpl);
   COLmap<COLstring, bool> Lookup;
   for(const auto& it : Roles) { if(pRoleMap->m_Roles.count(it)) { Lookup.add(it, true); } }
   if(Lookup.empty()) {
      COL_TRC("Unsecure -- can edit tags");
      return true;
   }
   for(const auto& it : Lookup) {
      if(ROLcheck(pRoleMap, it.first, ROLid::EDIT_TAGS)) {
         COL_TRC("Can still edit tags");
         return true;
      }
   }
   COL_TRC("Lost edit tags permission");
   return false;
}

// /role/test
void ROLtest(const COLwebRequest& Request, ROLmap* pRoleMap) {
   COL_FUNCTION(ROLcheck);
   COL_VAR(Request.Data);
   const COLvar& TagVar = COLvarRequiredObject(Request.Data, "tags");
   if(!TagVar.isArray()) { return COLrespondError(Request.Address, "Invalid tags data"); }
   if(Request.User == "admin") { return COLrespondSuccess(Request.Address); } // admin always has access
   COLarray<COLstring> Tags;
   for(const auto& it : TagVar.array()) { Tags.push_back(it); }
   ROLtestImpl(Tags, pRoleMap) ? COLrespondSuccess(Request.Address) : COLrespondError(Request.Address, "Losing permission to edit tags for this component");
}
