//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: PRMcomponentAll.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   02/02/24 12:37 PM
//  ---------------------------------------------------------------------------
#include <PRM/PRMcomponentAll.h>
#include <PRM/PRMcallbacks.h>

#include <CFG/CFGconfig.h>
#include <CFG/CFGmap.h>

#include <ROL/ROLid.h>
#include <ROL/ROLserialize.h>

#include <COL/COLlog.h>
#include <COL/COLweb.h>
COL_LOG_MODULE;

// /component/permission/all
void PRMcomponentAll(const COLwebRequest& Request, CFGmap* pMap) {
   COL_FUNCTION(PRMcomponentAll);
   COLvar Result;
   COLstring Error;
   for(const auto& it : pMap->ConfigMap) {
      ROLrole Role;
      if(!PRMcheckRolePermissions(false, Request.User, it->first, &Role, &Error)) {
         Result["errors"].push_back(it->second.CoreConfig.Name + ": " + Error);
      } else {
         Result["components"][it->first] = ROLroleToVar(Role);
      }
   }
   COLrespondSuccess(Request.Address, Result);
}