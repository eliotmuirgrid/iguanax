//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPdirRename
//
// Description:
//
// Implementation
//
// Author: John Q
//---------------------------------------------------------------------------
#include <CFG/CFGconfigApi.h>
#include <CFG/CFGmap.h>
#include <CMP/CMPdirRename.h>
#include <CMP/CMPutils.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
COL_LOG_MODULE;

static void CMPdirRenameImpl(const COLstring& Component, const COLstring& Path, const COLstring& NewPath, const bool& IsDev, const COLaddress& Address) {
   COL_FUNCTION(CMPdirRenameImpl);
   COL_VAR2(Path, NewPath);
   COLstring Error;
   if (CMPbadNewName(Path, NewPath, &Error)) {
      return COLrespondError(Address, Error);
   }
   COLstring OldDirPath =  CFGresolveRepoFile(Component, Path, IsDev);
   if(!FILfileExists(OldDirPath)) {
      COL_TRC("The directory you are trying to rename doesn't exist");
      return COLrespondError(Address, "The directory you are trying to rename no longer exists");
   }
   COLstring NewDirPath  = CFGresolveRepoFile(Component, NewPath, IsDev);
   COLstring MangledPath = CFGresolveRepoFile(Component, "CMPrandc1234domName43", IsDev);
   if(FILfileExists(NewDirPath) && !CMPmangleAndTryRename(OldDirPath, NewDirPath, MangledPath)) {
      COL_TRC("A directory with the new name already exists");
      COL_VAR(NewDirPath);
      return COLrespondError(Address, "A file or folder with this name already exists");
   }
   FILrenameFile(OldDirPath, NewDirPath);
   COLrespondSuccess(Address);
}

// "/component/dir/rename"
void CMPdirRename(const COLwebRequest& Request) {
   COL_FUNCTION(CMPdirRename);
   const COLvar& Data = Request.Data;
   COL_VAR(Data);
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

   CMPdirRenameImpl(
      ComponentId,
      COLvarRequiredString(Data, "old_path"),
      COLvarRequiredString(Data, "new_path"),
      Development,
      Request.Address);
}
