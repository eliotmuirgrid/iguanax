// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNideDirectories
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Tuesday 25 April 2023 - 12:05PM
// ---------------------------------------------------------------------------

#include <TRN/TRNideDirectories.h>

#include <PRM/PRMcallbacks.h>

#include <ROL/ROLid.h>

#include <CFG/CFGconfigApi.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <FIL/FILutils.h>
COL_LOG_MODULE;

static void TRNideDirectoriesImpl(const COLstring& ComponentId, const bool Dev, COLvar& Result){
   COL_FUNCTION(TRNideDirectoriesImpl);
   COLstring SourceDir = CFGresolveRepoFile(ComponentId, "", Dev);
   SourceDir.replace(FILworkingDir(), "");   // TODO - gross
   Result["SourceDir"]  = SourceDir; 
   Result["WorkingDir"] = FILworkingDir();
   COL_VAR(Result);
}

// /ide/directories
void TRNideDirectories(const COLwebRequest& Request) {
   COL_FUNCTION(TRNideDirectories);
   const COLvar& Input = Request.Data;
   bool      Dev         = COLvarRequiredBool  (Input, "development");
   COLstring ComponentId = COLvarRequiredString(Input, "component");
   COL_VAR(Request.Data);
   COLstring Error;
   ROLid RoleId = Dev ? ROLid::EDIT_SCRIPT : ROLid::VIEW_SCRIPT;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, ComponentId, RoleId, &Error)){
      COLstring Message = Dev ? "edit" : "view";
      return COLrespondError(Request.Address, PRMsetError(RoleId));
   }
   COLvar Result;
   TRNideDirectoriesImpl(ComponentId, Dev, Result);
   COLrespondSuccess(Request.Address, Result);
}