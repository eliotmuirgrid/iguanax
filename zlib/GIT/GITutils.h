#ifndef __GIT_UTILS_H__
#define __GIT_UTILS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITutils
//
// Author: Matthew Sobkowski 
// Date:   Monday 04 November 2024 - 09:59AM
// ---------------------------------------------------------------------------
class COLstring;

bool GITisPlainFolder(const COLstring& ComponentId);
void GITcleanUpGitDir(const COLstring& Dir, bool RemoveModules=true);
void GITcleanupAllGitDirs(const COLstring& RootDir, bool RemoveModules=true);

#endif // end of defensive include
