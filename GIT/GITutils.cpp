// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITutils
//
// Author: Matthew Sobkowski 
// Date:   Monday 04 November 2024 - 09:59AM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <DIR/DIRpath.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
COL_LOG_MODULE;

bool GITisPlainFolder(const COLstring& ComponentId) {
	COL_FUNCTION(GITisPlainFolder);
	const COLstring ComponentDir = DIRcomponentDevDir(ComponentId);
	return !FILfileExists(ComponentDir + ".git");
}

void GITcleanUpGitDir(const COLstring& Dir, bool RemoveModules) {
	COL_FUNCTION(GITcleanUpGitDir);
	const COLstring GitDir	   = FILpathAppend(Dir, ".git");
	const COLstring GitModules = FILpathAppend(Dir, ".gitmodules");
	COLstring RemoveError;
	if(FILfileExists(GitDir) && !FILremoveFullDirNewSafe(GitDir, &RemoveError)){
		COL_ERR(RemoveError);
	}
	if(RemoveModules && FILfileExists(GitModules)) { FILremove(GitModules); }
}

void GITcleanupAllGitDirs(const COLstring& RootDir, bool RemoveModules) {
	COL_FUNCTION(GITcleanupAllGitDirs);
	COL_VAR(RootDir);
	GITcleanUpGitDir(RootDir, RemoveModules);
	FILdirEnumerator Enum(FILpathAppend(RootDir, FIL_ANYFILE));
	COLstring FileName;
	while(Enum.getFile(FileName)) {
		const COLstring Path = FILpathAppend(RootDir, FileName);
		if(FILisDirectory(Path)) { GITcleanUpGitDir(Path, RemoveModules); }
	}
}