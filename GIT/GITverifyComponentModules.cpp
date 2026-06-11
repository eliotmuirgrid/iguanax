//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GITverifyComponentModules.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   27/05/25 11:36 AM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <GIT/GITverifyComponentModules.h>
#include <GIT/GITverifyModulesFile.h>
#include <PRM/PRMcallbacks.h>
COL_LOG_MODULE;

// /git/verify_component_modules
void GITverifyComponentModules(const COLwebRequest& Request) {
   COL_FUNCTION(GITverifyComponentModules);
   const COLstring Component = COLvarRequiredString(Request.Data, "component");
   COLstring Error;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, Component, VIEW_SCRIPT, &Error)){
      return COLrespondError(Request.Address, PRMsetError(VIEW_SCRIPT));
   }
   const bool      IsDev     = COLvarRequiredBool(Request.Data, "development");
   const COLstring Dir       = DIRcomponentProjectDir(Component, IsDev);
   COLstring       Err;
   const bool Valid = GITverifyModulesFile(Dir, &Err);
   COL_VAR2(Valid, Err);
   COLvar     Result;
   if(!Valid) { Result["error"] = Err; }
   COLrespondSuccess(Request.Address, Result);
}
