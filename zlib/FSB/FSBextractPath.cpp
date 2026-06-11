//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: FSBextractPath
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

#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLlog.h>
#include <COL/COLweb.h>
COL_LOG_MODULE;

void FSBfileBrowserExtractPathImp(COLvar& Result, const COLstring& Path) {
   COL_FUNCTION(FSBfileBrowserExtractPathImp);
   COL_VAR(Path);
	COLstring Filepath = Path;
 #ifdef _WIN32
   if ("/" == Filepath) Filepath = "C:\\";
   COL_TRC("Converted / to C:\\");
 #endif
   Filepath = FILexpand(Filepath);
   COL_TRC("After expand: " << Filepath);
   Filepath = FILpathResolve(Filepath, ""); 
   COL_TRC("After resolve relative: " << Filepath);
   while(!Filepath.is_null() && !FILfileExists(Filepath)){  // find a valid parent
      COL_TRC(Filepath << " doest not exist");
      Filepath = FILparentDir(Filepath);
      COL_TRC("Checking its parent " << Filepath);
   }
   if(Filepath.is_null()){
      COL_TRC("Path is invalid, default to root");
      Filepath = FILexpand("~/");
   }
   if (FILisDirectory(Filepath)) {
      Filepath = FILpathSepAdd(Filepath);
      COL_TRC(Filepath << " is directory, we are good");
   } else {
      COL_TRC(Filepath << " is file");
      Filepath = FILpathDir(Filepath);
      COL_TRC("Resolve to its parent directory " << Filepath);
   }
   COL_VAR(Filepath);
   Result["path"] = Filepath;
}

// "/file_browser/extract_path"
void FSBfileBrowserExtractPath(const COLwebRequest& Request) {
   COL_FUNCTION(FSBfileBrowserExtractPath);
   COLvar Data = Request.Data;
   COL_VAR(Data);
   COLvar Result;
   FSBfileBrowserExtractPathImp(Result, COLvarGet(Data, "path", "~/"));
   COL_VAR(Result.json(1));
   COLrespondSuccess(Request.Address, Result);
}