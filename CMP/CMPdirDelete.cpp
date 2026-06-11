//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPdirDelete
//
// Description:
//
// Implementation
//
// Author: Tyler Brown
//---------------------------------------------------------------------------
#include <CMP/CMPdirDelete.h>
#include <CMP/CMPutils.h>
#include <CFG/CFGmap.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <FIL/FILutils.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
COL_LOG_MODULE;

static void CMPdirDeleteImpl(const COLstring& Component, const COLstring& Path, const bool& IsDev, const COLaddress& Address) {
   COL_FUNCTION(CMPdirDeleteImpl);
   COLvar Result;
   COLstring FilePath;
   if(!CMPcomponentFileExists(Component, Path, IsDev, &FilePath, &Result)) { return COLrespondError(Address, "Could not resolve path."); }
   if(!FILfileExists(FilePath)) {
      return COLrespondError(Address, "A folder with this name does not exist");
   } else if(!FILisDirectory(FilePath)) {
      return COLrespondError(Address, "You are trying to remove a file");
   } else {
      COLstring Error;
      if (!FILremoveFullDirNewSafe(FilePath, &Error)){ return COLrespondError(Address, Error); }
      CMPremoveLuaRequireIfNeeded(Component, Path + "."); // append a dot to the path to prevent removal of file-specific requires if they have the same name
      return COLrespondSuccess(Address);
   }
}

// "/component/dir/delete"
void CMPdirDelete(const COLwebRequest& Request) {
   COL_FUNCTION(CMPdirDelete);
   COLvar Data = Request.Data;
   COLstring ComponentId, Error;
   bool Development = COLvarRequiredBool(Data, "development");
   if(!Development){
      return COLrespondError(Request.Address, "Cannot modify file structure of a run repository.");
   }
   if (!CFGcomponentId(Data, &ComponentId, &Error)){
      return COLrespondError(Request.Address, Error);
   }
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, ComponentId, ROLid::EDIT_SCRIPT, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_SCRIPT));
   }
   COL_VAR(Data);
   CMPdirDeleteImpl(
      ComponentId,
      COLvarRequiredString(Data, "path"),
      Development,
      Request.Address);
}
