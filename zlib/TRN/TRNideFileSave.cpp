//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNideFileSave
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Wed, July 20th, 2022
//---------------------------------------------------------------------------
#include <TRN/TRNideFileSave.h>
#include <TRN/TRNdecodeSampleDataFile.h>

#include <CFG/CFGconfigApi.h>
#include <CFG/CFGmap.h>

#include <FIL/FILutils.h>

#include <PRM/PRMcallbacks.h>

#include <ROL/ROLid.h>

#include <COL/COLweb.h>
#include <COL/COLlocker.h>
#include <COL/COLmutex.h>
#include <COL/COLvarUtils.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
#include <SCK/SCKcall.h>
COL_LOG_MODULE;

// TODO - is this really necessary?
static COLmutex* pFileSaveLock = new COLmutex;

static void TRNsetCompileFlagOnMain(const COLstring& Component){
   COL_FUNCTION(TRNsetCompileFlagOnMain);
   COLvar SetCompileRequest;
   SetCompileRequest["component"]   = Component;
   SetCompileRequest["development"] = true;
   COLvar SetCompileResult;
   COL_TRC("About to call /ide/compile/set on main thread");
   COL_VAR(SetCompileRequest);
   bool CallSetCompile = SCKcall("/ide/compile/set", SetCompileRequest, COLstring(), &SetCompileResult, false);
   if (!CallSetCompile) { COL_TRC("Failed to call /ide/compile/set. Make sure the api is registered."); }
   COL_TRC("/ide/compile/set returned");
   COL_VAR(SetCompileResult);
}

static void TRNideFileSaveImpl(const COLstring& Component, const COLstring& FileName, COLstring& Content, const bool& IsDev, const COLaddress& Address){
   COL_FUNCTION(TRNideFileSaveImpl);
   COL_VAR(FileName);
   COLstring Path = CFGresolveRepoFile(Component, FileName, IsDev);
   if (Path.is_null()){
      COLrespondError(Address, "Invalid file name.");
      return;
   }
   // TODO - this magic should be a front end concern - we should send binary data back and forth.
   // TRNdecodeSampleDataFile(Path, Component, &Content);
   COL_TRC_DUMP("Dump", Content.c_str() , Content.size());
   {
      COLlocker FileLock(*pFileSaveLock);
      COL_TRC("Saving: " << Path);
      FILwriteFile(Path, Content);
      COL_TRC("Save complete, sending a compile request");
   }
   TRNsetCompileFlagOnMain(Component);
   COLvar Data;
   Data["message"] = "Saved file.";
   COLrespondSuccess(Address, Data);
}

// /file/save
void TRNideFileSave(const COLwebRequest& Request) {
   COL_FUNCTION(TRNideFileSave);
   COL_VAR(Request.Data.json(1));
   COLstring ComponentId, Error;
   COLstring Content   = COLvarRequiredString(Request.Data, "content");
   COLstring FileName  = COLvarRequiredString(Request.Data, "file");  // relative path
   bool Development      = COLvarRequiredBool(Request.Data, "development");
   if(!Development) { return COLrespondError(Request.Address, "Cannot modify file structure of a run repository."); }
   if (!CFGcomponentId(Request.Data, &ComponentId, &Error)){
      return COLrespondError(Request.Address, Error);
   }
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, ComponentId, ROLid::EDIT_SCRIPT, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_SCRIPT));
   }
   TRNideFileSaveImpl(ComponentId, FileName, Content, Development, Request.Address);
}
