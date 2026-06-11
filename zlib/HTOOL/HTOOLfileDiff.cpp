// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: HTOOLfileDiff.cpp
//
//  Description
//
// Author: Shuo Hou
// Date:   June 22 2022
//---------------------------------------------------------------------------
#include <CFG/CFGconfigApi.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GIT/GITshow.h>
#include <GIT/GITutils.h>
#include <HTOOL/html_diff.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
COL_LOG_MODULE;

bool HTOOLcheckDiffParameters(const COLstring& FileName, const COLstring& FilePath, const bool& FileExist,
										COLvar* pResult) {
	COL_FUNCTION(HTOOLcheckDiffParameters);
	COL_VAR(FilePath);
	if(FileExist && FILisDirectory(FilePath)) {
		(*pResult)["status"] = FileName + " is a directory.";
		return false;
	}
	if(FileExist && FILfileSize(FilePath) > 1048576) {
		(*pResult)["status"] = FileName + " is too large to compare.";
		return false;
	}
	return true;
}

void HTOOLnewFileContent(const COLstring& ProjectPath, const COLstring& CommitId, const COLstring FileName,
								 COLstring* pContent) {
	COL_FUNCTION(HTOOLnewFileContent);
	if(CommitId.isWhitespace()) {
		COLstring FilePath = FILpathAppend(ProjectPath, FileName);
		HTOOLreadAsciiFile(FilePath, pContent);  // deleted file shows as empty string
		COL_TRC("Current file content:\n" + *pContent);
		return;
	}
	COLstring Arg, Error;
	Arg = CommitId + ":" + FILpathSepToPosix(FileName); // git won't work without posix style paths
	GITshow(ProjectPath, Arg, pContent, &Error);
}

void HTOOLoldFileContent(const COLstring& ProjectPath, const COLstring& CommitId, const COLstring FileName,
								 COLstring* pContent) {
	COL_FUNCTION(HTOOLoldFileContent);
	COLstring Arg, Error;
	if(CommitId.isWhitespace()) {
		Arg = "HEAD";
	} else {
		Arg = CommitId + "~";
	}
	Arg = Arg + ":" + FILpathSepToPosix(FileName); // git won't work without posix style paths
	GITshow(ProjectPath, Arg, pContent, &Error);
	// You may need the NewFile bool - leaving traces of the logic.
	// if(!GITshow(ProjectPath, Arg, &FileAContent, &Error)){
	//    if(Error.find("exists on disk, but not in") != npos) { NewFile = true; }
	//    else { Result["status"] = Error; }
	// }
}

void HTOOLgetDiffContent(const COLstring& ProjectPath, const COLstring& CommitId, const COLstring& FileName,
								 COLvar* pResult) {
	COL_FUNCTION(HTOOLgetDiffContent);
	COLstring FileAContent, FileBContent;	// A is previous commit, B is current
	// This may be needed in the new and old file content functions
	bool NewFile = false;
	COLstring Error;
	COLstring Arg = CommitId + ":" + FileName;
	HTOOLnewFileContent(ProjectPath, CommitId, FileName, &FileBContent);
	HTOOLoldFileContent(ProjectPath, CommitId, FileName, &FileAContent);
	HTOOLhtmldiffFileContent(FileAContent, Arg, FileBContent, FileName, pResult);
	if(NewFile && FileBContent.size() == 0 && (*pResult)["status"].asString() == "No changes.") {
		(*pResult)["status"] = "Newly added empty file.";
	}
}

static void HTOOLfileDiffImpl(const COLstring& Component, const COLstring& ProjectPath, const COLstring& FileName,
										const COLstring& RelativePath, const COLstring CommitId, const COLaddress& Address) {
	COL_FUNCTION(HTOOLfileDiffImpl);
	COLvar Result;
	COLstring FilePath = FILpathAppend(ProjectPath, FileName);
	bool FileExists	 = FILfileExists(FilePath);
	COL_VAR3(ProjectPath, FileName, FileExists);
	if(!HTOOLcheckDiffParameters(FileName, FilePath, FileExists, &Result)) { return COLrespondSuccess(Address, Result); }
	HTOOLgetDiffContent(ProjectPath, CommitId, FileName, &Result);
	COLrespondSuccess(Address, Result);
	COL_VAR(Result.json(1));
}

// if we have a plain folder, we need to fix the paths so the root repo can reference the correct file
static void HTOOLfixPaths(const COLstring& Component, COLstring* pProjectPath, COLstring* pFileName) {
	if(!GITisPlainFolder(Component)) { return; }
	*pProjectPath = DIRconfigFile("");
	*pFileName = FILpathAppend("comps/" + Component + "/dev/", *pFileName);
}

// /htool/file_diff
void HTOOLfileDiff(const COLwebRequest& Request) {
	COL_FUNCTION(HTOOLfileDiff);
	COL_VAR(Request.Data.json(1));
	const COLstring Component = COLvarRequiredString(Request.Data, "component");
	COLstring Error;
	if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, Component, ROLid::EDIT_SCRIPT, &Error)) {
		return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_SCRIPT));
	}
	const COLstring CommitId = COLvarGetString(Request.Data, "commit", "");
	COLstring FileName		 = COLvarRequiredString(Request.Data, "file_name");
	COLstring RelativePath	 = COLvarGetString(Request.Data, "relative_path", "");
	COLstring ProjectPath	 = CFGresolveRepoFile(Component, "", true);
	if(ProjectPath.is_null()) { return COLrespondError(Request.Address, ProjectPath); }
	ProjectPath = FILpathAppend(ProjectPath, RelativePath);

	HTOOLfixPaths(Component, &ProjectPath, &FileName);
	HTOOLfileDiffImpl(Component, ProjectPath, FileName, RelativePath, CommitId, Request.Address);
}
