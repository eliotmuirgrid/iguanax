// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: PRMcomponent
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Monday 29 January 2024 - 07:53AM
// ---------------------------------------------------------------------------

#include <PRM/PRMcomponent.h>
#include <PRM/PRMcallbacks.h>
#include <CFG/CFGconfig.h>
#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>
#include <ROL/ROLid.h>
#include <ROL/ROLserialize.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
COL_LOG_MODULE;

void PRMcomponentImpl(const COLstring& Component, const COLstring& User, bool FromTranslator, COLaddress Address){
   COL_FUNCTION(PRMcomponentImpl);
   COLstring Error;
   ROLrole Role;
   if(!PRMcheckRolePermissions(FromTranslator, User, Component, &Role, &Error)) {
      return COLrespondError(Address, "Could not resolve permissions");
   }
   Component.is_null() ? COLrespondSuccess(Address, ROLroleToVarIguanaOnly(Role)) : COLrespondSuccess(Address, ROLroleToVar(Role));
}

// /component/permission
void PRMcomponent(const COLwebRequest& Request) {
   COL_FUNCTION(PRMcomponent);
   COLstring ComponentId, Error;
   COLstring User      = Request.User;
   ComponentId = COLvarGetString(Request.Data, "component", "");
   if(!ComponentId.is_null()){
      if (!CFGcomponentId(Request.Data, &ComponentId, &Error)){
         return COLrespondError(Request.Address, Error);
      }
   }
   PRMcomponentImpl(ComponentId, User, Request.FromTranslator, Request.Address);
}
