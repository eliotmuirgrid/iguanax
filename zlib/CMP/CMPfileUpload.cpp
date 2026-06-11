// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPfileUpload
//
// Description:
//
// Implementation
//
// Author: Orkhan Amikishiyev 
// Date:   Thursday 02 March 2023 - 04:30PM
// ---------------------------------------------------------------------------
#include <CMP/CMPfileUpload.h>
#include <CMP/CMPutils.h>
#include <CFG/CFGmap.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SMPL/SMPLencoding.h>
#include <TRN/TRNdecodeSampleDataFile.h>
COL_LOG_MODULE;

static void CMPfileUploadImpl(const COLstring& Component, const COLstring& Path, COLstring& Content, bool IsDev, const COLaddress& Address) {
   COL_FUNCTION(CMPfileUploadImpl);
   COLstring FilePath;
   COLvar Result;
   if(!CMPcomponentFileExists(Component, Path, IsDev, &FilePath, &Result)) { return COLrespondError(Address, "Could not resolve path"); }
   COLstring Extension = FILpathExtLast(Path, true);
   if (!TRNisAllowedExtension(Extension)) { return COLrespondError(Address, "Invalid file extension: " + Extension + ". Only specific file types are allowed."); }
   if(FILfileExists(FilePath)) {
      return COLrespondError(Address, "A file or folder with this name already exists");
   } else {
      FILcreateDirectoryTreeToFile(FilePath, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
      COLstring RawCode;
      SMPLdecode(Content, &RawCode);
      RawCode.swap(&Content);
      FILwriteFile(FilePath, Content);
      return COLrespondSuccess(Address);
   }
}

// "/component/file/upload"
void CMPfileUpload(const COLwebRequest& Request) {
   COL_FUNCTION(CMPfileUpload);
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
   COLstring Content = COLvarRequiredString(Data, "content");
   CMPfileUploadImpl(
      ComponentId,
      COLvarRequiredString(Data, "path"),
      Content,
      Development,
      Request.Address);
}