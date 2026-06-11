// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: EXPTprojectFiles
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Friday 14 November 2025 - 12:47PM
// ---------------------------------------------------------------------------
#include <EXPT/EXPTdirEnum.h>
#include <EXPT/EXPTprojectFiles.h>

#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <PRM/PRMcallbacks.h>

#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// /export/dev/files
void EXPTprojectFiles(const COLwebRequest& Request){
   COL_FUNCTION(EXPTprojectFiles);
   COL_VAR(Request.Data);
   COLstring Component = COLvarRequiredString(Request.Data, "component");
   bool Development    = COLvarRequiredBool(Request.Data, "development");

   COLstring Error;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, Component, ROLid::EDIT_SCRIPT, &Error)){
      COL_TRC("User missing edit script permission");
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_SCRIPT));
   }
   COLstring ComponentDir = DIRcomponentProjectDir(Component, Development);

   COLvar Result;
   COLstring StrContent;
   // First capture the important files
   EXPTcaptureSpecificFile(ComponentDir, "main.lua",    &StrContent, &Result["skipped"]);
   EXPTcaptureSpecificFile(ComponentDir, "config.json", &StrContent, &Result["skipped"]);
   EXPTcaptureSpecificFile(ComponentDir, ".gitmodules", &StrContent, &Result["skipped"]);
   // Then get the rest
   EXPTgetAllFilesStrRecursive(ComponentDir, ComponentDir, true, &StrContent, &Result["skipped"]);
   Result["strdata"] = StrContent;
   COL_TRC("Read successful");

   return COLrespondSuccess(Request.Address, Result);
}