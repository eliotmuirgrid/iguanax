//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPfileAdd
//
// Description:
//
// Implementation
//
// Author: Tyler Brown
//---------------------------------------------------------------------------
#include <CMP/CMPfileAdd.h>
#include <CMP/CMPutils.h>
#include <CFG/CFGmap.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <TRN/TRNdecodeSampleDataFile.h>
COL_LOG_MODULE;

static void CMPfileAddImpl(const COLstring& Component, COLstring Path, COLstring& Content, const bool& IsDev, const COLaddress& Address) {
   COL_FUNCTION(CMPfileAddImpl);
   COLstring Error, FilePath;
   Path = FILpathSepToNative(Path);    // / and \ work on Windows and POSIX
   if (FILpathIsInvalid(Path, &Error)) {
      return COLrespondError(Address, Error);
   }
   COLvar Result;
   if(!CMPcomponentFileExists(Component, Path, IsDev, &FilePath, &Result)) { return COLrespondError(Address, "Could not resolve path."); }
   if(FILfileExists(FilePath)) {
      return COLrespondError(Address, "A file or folder with this name already exists");
   } else {
      FILcreateDirectoryTreeToFile(FilePath, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
      TRNdecodeSampleDataFile(FilePath, Component, &Content);
      FILwriteFile(FilePath, Content);
      return COLrespondSuccess(Address);
   }
}

// "/component/file/add"
void CMPfileAdd(const COLwebRequest& Request) {
   COL_FUNCTION(CMPfileAdd);
   COLvar Data = Request.Data;
   COL_VAR(Data);
   COLstring ComponentId, Error;
   COLstring Content  = COLvarRequiredString(Data, "content");
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
   CMPfileAddImpl(
      ComponentId,
      COLvarRequiredString(Data, "path"),
      Content,
      Development,
      Request.Address);
}
