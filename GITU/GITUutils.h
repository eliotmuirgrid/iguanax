#ifndef __GITU_UTILS_H__
#define __GITU_UTILS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUutils
//
// Description:
//
// Helper routines to be used in GIT
//
// Author: Vismay Shah
// Date:   Wednesday 14 August 2024 - 09:43AM
// ---------------------------------------------------------------------------
#include <COL/COLstring.h>
class INIdata;

COLstring GITUquoteArgument(const COLstring& Arg);

void GITUreadGitFile(const COLstring& FileName, INIdata* pData);
void GITUsaveGitFile(const COLstring& FileName, const INIdata& Data);

bool GITUvalidLibraryName(const COLstring& Name, COLstring* pError);
bool GITUvalidComponentName(const COLstring& Name, COLstring* pError);

COLstring GITUresolveKeyFromBranchUrl(const COLstring& Url, const COLstring& Branch);

#endif // end of defensive include
