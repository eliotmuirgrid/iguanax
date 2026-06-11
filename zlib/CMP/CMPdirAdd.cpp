//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPdirAdd
//
// Description:
//
// Implementation
//
// Author: Tyler Brown
//---------------------------------------------------------------------------
#include <CMP/CMPdirAdd.h>
#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>
#include <CMP/CMPutils.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
COL_LOG_MODULE;

static void CMPdirAddImpl(const COLstring& Component, COLstring Path, const bool& IsDev, const COLaddress& Address) {
   COL_FUNCTION(CMPdirAddImpl);
   COLvar Result;
   COLstring Error, FilePath;
   Path = FILpathSepToNative(Path);    // / and \ work on Windows and POSIX
   if (FILpathIsInvalid(Path, &Error)) {
      return COLrespondError(Address, Error);
   }
   if(!CMPcomponentFileExists(Component, Path, IsDev, &FilePath, &Result)) { return COLrespondError(Address, "Could not resolve path."); }
   COLstring DirPath = FILpathDir(FilePath);
   if(FILfileExists(DirPath)) {
      return COLrespondError(Address, "Directory creation failed. A dir with that path exists: \n\n" + FilePath);
   } else {
      FILmakeFullDir(DirPath, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
      return COLrespondSuccess(Address);
   }
}

// "/component/dir/add"
void CMPdirAdd(const COLwebRequest& Request) {
   COL_FUNCTION(CMPdirAdd);
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
   CMPdirAddImpl(
      ComponentId,
      COLvarRequiredString(Data, "path"), // Path gets resolved to last directory so include trailing slash if needed
      Development,
      Request.Address);
}
