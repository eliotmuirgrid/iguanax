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
#include <CFG/CFGconfigApi.h>
#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>
#include <CMP/CMPutils.h>
#include <CMPC/CMPCutils.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
COL_LOG_MODULE;

bool CMPcomponentFileExists(const COLstring& Component, const COLstring& RelativePath, const bool& IsDevelopment, COLstring* pResultPath, COLvar* pResult) {
   COL_FUNCTION(CMPcomponentFileExists);
   *pResultPath = CFGresolveRepoFile(Component, RelativePath, IsDevelopment);
   if (pResultPath->is_null()) {
      (*pResult)["success"] = false;
      (*pResult)["error"] = "Could not resolve path to: " + *pResultPath;
      return false;
   }
   return true;
}

void CMPremoveLuaRequireIfNeeded(const COLstring& Component, const COLstring& Path) {
   COL_FUNCTION(CMPremoveLuaRequireIfNeeded);
   COL_VAR2(Component, Path);
   COLstring LuaRequire = Path;
   LuaRequire.replace(".lua", "");
   LuaRequire.replace(FIL_DIR_SEPARATOR, ".");
   COL_VAR(LuaRequire);
   CMPCremoveRequireFromAllFiles(DIRcomponentDevDir(Component), LuaRequire);
}

bool CMPbadNewName(const COLstring& Path, const COLstring& NewPath, COLstring* pError) {
   if (FILpathName(Path) == FILpathName(NewPath)) {
      COL_TRC("Moving '" << Path << "' to '" << NewPath << "'. No need to check.");
      return false;
   }
   COLstring ParentDir = FILparentDir(Path);
   COLstring NewName(NewPath);
   if (!ParentDir.is_null()) {
      NewName.shift(ParentDir.size() + 1);
   }
   COL_VAR(NewName);
   return FILnameIsInvalid(NewName, pError);
}

bool CMPmangleAndTryRename(const COLstring& OldDirPath, const COLstring& NewDirPath, const COLstring& MangledPath){
   COL_FUNCTION(CMPmangleAndTryRename);
   FILrenameFile(OldDirPath, MangledPath);
   bool Success = !FILfileExists(NewDirPath);
   FILrenameFile(MangledPath, OldDirPath);
   return Success;
}