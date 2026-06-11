// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: EXPTsampleData
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Friday 14 November 2025 - 12:47PM
// ---------------------------------------------------------------------------
#include <EXPT/EXPTdirEnum.h>
#include <EXPT/EXPTsampleData.h>

#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>
#include <PRM/PRMcallbacks.h>

#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static bool EXPTcheckSampleDir(const COLstring& SampleDir, COLstring* pError){
   COL_FUNCTION(EXPTcheckSampleDir);
   if (!FILfileExists(SampleDir)) { 
      *pError = "This component has no Samples directory.";
      return false;
   }
   if (FILisDirectoryEmpty(SampleDir)){
      *pError = "This component has no sample data files.";
      return false;
   }
   COL_TRC("Samples directory exists and is not empty");
   return true;
}

// /export/dev/samples
void EXPTsampleData(const COLwebRequest& Request){
   COL_FUNCTION(EXPTsampleData);
   COL_VAR(Request.Data);
   COLstring Component = COLvarRequiredString(Request.Data, "component");
   bool Development    = COLvarRequiredBool(Request.Data, "development");

   COLstring Error;
   ROLid RoleId = Development ? ROLid::EDIT_SCRIPT : ROLid::VIEW_SCRIPT;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, Component, RoleId, &Error)){
      COL_TRC("User missing correct permission");
      return COLrespondError(Request.Address, PRMsetError(RoleId));
   }
   COLstring Dir = DIRcomponentProjectDir(Component, Development);
   COLstring SampleDir = FILpathAppend(Dir, "Samples/");
   COLvar Result;
   Result["strdata"] = "";
   if (!EXPTcheckSampleDir(SampleDir, &Error)){
      return COLrespondSuccess(Request.Address, Result);
   }
   COLstring StrContent;
   EXPTgetAllFilesStrRecursive(Dir, SampleDir, false, &StrContent, &Result["skipped"]);
   Result["strdata"] = StrContent;
   return COLrespondSuccess(Request.Address, Result);
}
