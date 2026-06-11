// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNfileFind
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Monday 01 May 2023 - 10:28AM
// ---------------------------------------------------------------------------

#include <TRN/TRNfileFind.h>
#include <TRN/TRNfile.h>

#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

TRNfile* TRNfileFindOrAdd(COLhashmap<COLstring, TRNfile*>& LuaFileList, const COLstring& ProjectPath, const COLstring& FriendlyPath){
   COL_FUNCTION(TRNfileFindOrAdd);
   COLstring SimpleFriendlyPath, PathErr;
   FILpathSimplify(FriendlyPath, &SimpleFriendlyPath, &PathErr);
   COL_VAR3(ProjectPath, FriendlyPath, SimpleFriendlyPath);
   COLhashmapPlace i = LuaFileList.find(SimpleFriendlyPath);
   if (!i) {
      COL_TRC("Didn't find it...");
      COLstring FilePath;
      FilePath = ProjectPath;
      FILaddPathSeparator(FilePath);
      FilePath += FriendlyPath;
      FILcorrectPathSeparators(FilePath);
      COL_VAR2(FriendlyPath, FilePath);
      FilePath = FILpathResolve(FilePath);
      i = LuaFileList.add(SimpleFriendlyPath, new TRNfile(FilePath, SimpleFriendlyPath));
      COL_TRC("Created Lua file " << SimpleFriendlyPath);
   } else { COL_TRC("Exists already.");}
   COL_VAR(LuaFileList.size());
   return (LuaFileList)[i];
}

const TRNfile* TRNfileFind(const COLhashmap<COLstring, TRNfile*>& LuaFileList, const COLstring& Label) {
   COL_FUNCTION(TRNfileFind);
   COL_VAR(Label);
   COLstring SimpleFriendlyPath, PathErr;
   FILpathSimplify(Label, &SimpleFriendlyPath, &PathErr);
   COLhashmapPlace i = LuaFileList.find(SimpleFriendlyPath);
   TRNfile* pFile = i ? LuaFileList[i] : NULL;
   COL_VAR((void*)pFile);
   return pFile;
}

bool TRNfileLoaded(COLhashmap<COLstring, TRNfile*>& LuaFileList, const COLstring& FriendlyPath){
   COL_FUNCTION(TRNfileLoaded);
   bool Result = LuaFileList.find(FriendlyPath) != NULL;
   COL_VAR2(FriendlyPath, Result);
   return Result;
}
