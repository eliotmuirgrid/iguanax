//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPfileRename
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
//---------------------------------------------------------------------------
#include <CFG/CFGconfigApi.h>
#include <CFG/CFGmap.h>
#include <CMP/CMPfileRename.h>
#include <CMP/CMPutils.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
COL_LOG_MODULE;

static void CMPfileRenameImpl(const COLstring& Component, const COLstring& Path, const COLstring& NewPath, const bool& IsDev, const COLaddress& Address) {
   COL_FUNCTION(CMPfileRenameImpl);
   COL_VAR2(Path, NewPath);
   COLstring Error;
   if (CMPbadNewName(Path, NewPath, &Error)) {
      return COLrespondError(Address, Error);
   }
   COLstring FilePathOld =  CFGresolveRepoFile(Component, Path, IsDev);
   if(!FILfileExists(FilePathOld)) {
      COL_TRC("The file you are trying to rename doesn't exist");
      return COLrespondError(Address, "The file you are trying to rename no longer exists");
   }
   COLstring FilePathNew = CFGresolveRepoFile(Component, NewPath, IsDev);
   COLstring MangledPath = CFGresolveRepoFile(Component, "CMPrandc1234domName43", IsDev);
   if(FILfileExists(FilePathNew) && !CMPmangleAndTryRename(FilePathOld, FilePathNew, MangledPath)) {
      COL_TRC("A file with the new name already exists");
      COL_VAR(FilePathNew);
      return COLrespondError(Address, "A file or folder with this name already exists");
   }
   // TODO - could we change to do a git mv ? Would be better for history.
   FILrenameFile(FilePathOld, FilePathNew);
   COLrespondSuccess(Address);
}

// "/component/file/rename"
void CMPfileRename(const COLwebRequest& Request) {
   COL_FUNCTION(CMPfileRename);
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
   CMPfileRenameImpl(
      ComponentId,
      COLvarRequiredString(Data, "old_path"),
      COLvarRequiredString(Data, "new_path"),
      Development,        // TODO - is dev is always true - we don't rename on non-dev repos
      Request.Address);
}
