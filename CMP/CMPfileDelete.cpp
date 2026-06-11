//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPfileDelete
//
// Description:
//
// Implementation
//
// Author: Tyler Brown
//---------------------------------------------------------------------------
#include <CMP/CMPfileDelete.h>
#include <CMP/CMPutils.h>
#include <CFG/CFGmap.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
COL_LOG_MODULE;

static void CMPfileDeleteImpl(const COLstring& Component, const COLstring& Path, const bool& IsDev,
                              const COLaddress& Address) {
   COL_FUNCTION(CMPfileDeleteImpl);
   COLstring FilePath;
   COLvar    Result;
   if(!CMPcomponentFileExists(Component, Path, IsDev, &FilePath, &Result)) {
      return COLrespondError(Address, "Could not resolve path.");
   }
   COL_VAR(FilePath);
   if(!FILfileExists(FilePath)) {
      return COLrespondError(Address, "A file with this name does not exist");
   } else if(FILisDirectory(FilePath)) {
      return COLrespondError(Address, "You are trying to remove a folder");
   }
   try {
      FILremoveWithThrow(FilePath, true);
   } catch(const COLerror& E) {
      COL_TRC(E.description());
      return COLrespondError(Address, E.description());
   }
   if(FILpathExtLast(Path, true) == ".lua") { CMPremoveLuaRequireIfNeeded(Component, Path); }
   COL_TRC("Successfully removed file");
   COLrespondSuccess(Address);
}

// "/component/file/delete"
void CMPfileDelete(const COLwebRequest& Request) {
   COL_FUNCTION(CMPfileDelete);
   COLstring ComponentId, Error;
   bool            Development = COLvarRequiredBool(Request.Data, "development");
   if(!Development) { return COLrespondError(Request.Address, "Cannot modify file structure of a run repository."); }
      if (!CFGcomponentId(Request.Data, &ComponentId, &Error)){
      return COLrespondError(Request.Address, Error);
   }
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, ComponentId, ROLid::EDIT_SCRIPT, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_SCRIPT));
   }
   COL_VAR(Request.Data);
   CMPfileDeleteImpl(ComponentId, COLvarRequiredString(Request.Data, "path"), Development, Request.Address);
}
