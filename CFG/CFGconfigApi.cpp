//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CFGconfigApi
//
// Implementation
//
// Author: Eliot Muir
//---------------------------------------------------------------------------
#include <CFG/CFGconfigLoad.h>
#include <CFG/CFGconfig.h>
#include <CFG/CFGconfigApi.h>
#include <COL/COLlog.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
COL_LOG_MODULE;

static COLstring CFGresolveHelper(const COLstring& Dir, const COLstring& RelativeFilePath) {
   COL_FUNCTION(CFGresolveHelper);
   COL_VAR2(Dir, RelativeFilePath);
   if(Dir.is_null() || Dir == DIRcompsDir() || Dir == FIL_DIR_SEPARATOR) {
      COL_TRC("Attempting to access above component root -- returning null");
      return "";
   }
   COLstring Source = FILpathResolve(Dir);
   Source = DIRcheckAboveRoot(Source, RelativeFilePath);
   COL_VAR(Source);
   return Source;
}

bool CFGdevelopmentMode(const COLstring& CommitId, const bool ForceDevelopment) {
	COL_FUNCTION(CFGdevelopmentMode);
	return CommitId.is_null() || CommitId == "DEVELOPMENT" || ForceDevelopment;
}

COLstring CFGresolveRepoFile(const COLstring& Component, const COLstring& RelativeFilePath, bool IsDevelopment){
   COL_FUNCTION(CFGresolveRepoFile);
   COL_VAR3(Component, RelativeFilePath, IsDevelopment);
	CFGconfig Config;
	if(!CFGconfigLoad(Component, &Config)) {
		COL_TRC("Component not found");
		return "";
	}
   COLstring Dir;
   if (CFGdevelopmentMode(Config.CoreConfig.Commit, IsDevelopment)){
      Dir = DIRcomponentDevDir(Component);
   } else {
      Dir = DIRcomponentRunDir(Component);
   }
   COL_VAR(Dir);
   return CFGresolveHelper(Dir, RelativeFilePath);
}
