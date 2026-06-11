//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPfileContent
//
// Description:
//
// Implementation
//
// Author: Tyler Brown
//---------------------------------------------------------------------------
#include <CMP/CMPfileContent.h>
#include <CMP/CMPutils.h>
#include <CFG/CFGmap.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
COL_LOG_MODULE;

static void CMPfileContentImpl(const COLstring& Component, const COLstring& Path, const bool& IsDev, COLvar& Result) {
   COL_FUNCTION(CMPfileContentImpl);
   COLstring FilePath;
   if(!CMPcomponentFileExists(Component, Path, IsDev, &FilePath, &Result)) { return; }
   COLvar Data;
   if(!FILfileExists(FilePath)) { Data["exists"] = false; }
   else if(FILisDirectory(FilePath)) {
      Data["exists"] = true;
      Data["dir"]    = true;
   } else {
      COLstring FileContents;
      FILreadFile(FilePath, &FileContents);
      Data["exists"]  = true;
      Data["dir"]     = false;
      Data["content"].swap(&FileContents);
   }
   Result = Data;
}

// "/component/file"
void CMPfileContent(const COLwebRequest& Request) {
   COL_FUNCTION(CMPfileContent);
   const COLvar& Data = Request.Data;
   COLstring ComponentId, Error;
   bool IsDev = COLvarRequiredBool(Data, "development");
   if (!CFGcomponentId(Data, &ComponentId, &Error)){
      return COLrespondError(Request.Address, Error);
   }
   ROLid Id = IsDev ? ROLid::EDIT_SCRIPT : ROLid::VIEW_SCRIPT;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, ComponentId, Id, &Error)){
      return COLrespondError(Request.Address, PRMsetError(Id));
   }

   COL_VAR(Data);
   COLvar Result;
   CMPfileContentImpl(
      ComponentId,
      COLvarRequiredString(Data, "path"),
      COLvarRequiredBool(Data, "development"),
      Result);
   COLrespondSuccess(Request.Address, Result);
}
