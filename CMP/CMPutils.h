#ifndef __CMP_UTILS_H__
#define __CMP_UTILS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPutils
//
// Description:
//
// Shared functions for generic component APIs.
//
// Author: Tyler Brown
//---------------------------------------------------------------------------
class COLstring;
class COLvar;
bool CMPcomponentFileExists(const COLstring& Component, const COLstring& RelativePath, const bool& IsDevelopment, COLstring* pResultPath, COLvar* pResult);
void CMPremoveLuaRequireIfNeeded(const COLstring& Component, const COLstring& Path);
// Checks if the filename part of NewPath is invalid when renaming Path to NewPath
// Returns false if filename part of NewPath matches that of Path (i.e. moving Path to NewPath)
// NOTE: doesn't handle renaming "foo" to "&*(/foo", where the bad new name triggers the skip moving file logic
bool CMPbadNewName(const COLstring& Path, const COLstring& NewPath, COLstring* pError);

bool CMPmangleAndTryRename(const COLstring& OldDirPath, const COLstring& NewDirPath, const COLstring& MangledPath);

#endif // end of defensive include
