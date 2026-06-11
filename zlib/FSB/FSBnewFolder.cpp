//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: FSBnewFolder
//
// Author: Matthew Nitsopoulos
//
// Description:
//
// Implementation
//---------------------------------------------------------------------------
#include "FSBvolumes.h"

#include <FIL/FILstorageManager.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>

#include <PRO/PROenv.h>

#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLlog.h>
#include <COL/COLweb.h>
COL_LOG_MODULE;

void FSBfileBrowserNewFolderImp(const COLaddress& Address, COLstring Path, const COLstring& FolderName) {
   COL_FUNCTION(FSBfileBrowserNewFolderImp);
   COLstring Error;
   if (FILnameIsInvalid(FolderName, &Error)) {
      return COLrespondError(Address, Error);
   }
   COL_VAR(Path);
   FILcorrectPathSeparators(Path);
   COL_VAR(Path);
   Path = FILexpand(Path);
   COL_VAR(Path);
   FILaddPathSeparator(Path);
   COL_VAR(Path);
   COLstring FullPath = Path + FolderName;
   COL_VAR(FullPath);
   COL_VAR(FILfileExists(FullPath));
   if(!FILfileExists(FullPath)){
      FILmakeFullDir(FullPath, FIL_USER_RWX);
   } else{
      return COLrespondError(Address, "There is already a file/dir with this name.");
   }
   COLrespondSuccess(Address);
}

// /file_browser/new_folder
// CAN we get rid of the folder argument - seems redundant.
void FSBfileBrowserNewFolder(const COLwebRequest& Request) {
   COL_FUNCTION(FSBfileBrowserNewFolder);
   COL_VAR(Request.Data);
   FSBfileBrowserNewFolderImp(Request.Address,
               COLvarRequiredString(Request.Data, "path"),
               COLvarRequiredString(Request.Data, "folder"));
}