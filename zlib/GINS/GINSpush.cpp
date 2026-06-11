// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GINSpush
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Wednesday 11 September 2024 - 12:37PM
// ---------------------------------------------------------------------------
#include <CFG/CFGmap.h>
#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>
#include <GINS/GINScommitInfo.h>
#include <GINS/GINSpush.h>
#include <GINS/GINSutils.h>
#include <GIT/GITpush.h>
#include <GIT/GITremote.h>
#include <PRM/PRMcallbacks.h>
#include <SCK/SCKloop.h>
#include <SCK/SCKworker.h>
COL_LOG_MODULE;

void GINSpushCallback(COLvar Result, GINScommitInfo* pCommitInfo, COLaddress Address) {
	COL_FUNCTION(GINSpushCallback);
	COL_VAR(Result.json(1));
	if(!Result["success"].asBool()) {
		GINSunsuccessfullPush(pCommitInfo, Result["error"]);
		return COLrespondError(Address, Result["error"]);
	}
	const COLstring Output = Result.exists("output") ? Result["output"].asString() : "";
	const COLstring LastWarning = Result["last_warning"].asString();
	pCommitInfo->LastWarning = LastWarning;
	if(!Output.is_null() && Output != "No push needed") { GINSsuccessfullPush(pCommitInfo, Result["last_warning"]); }
	COLrespondSuccess(Address, Output);
}

void GINSpushSetResult(COLvar* pOut, bool Success, const COLstring& Error="") {
	COL_FUNCTION(GINSpushSetResult);
	(*pOut)["success"]   = Success;
	if(!Error.is_null()) { (*pOut)["error"] = Error; }
}

static bool GINScheckGitRemote(const COLstring& GitPath, const COLstring& Name, COLstring* pError) {
    COLstring Remote;
    if (FILfileExists(GitPath) && !GITremote(GitPath, &Remote, pError)) {
        COL_TRC("Error getting remote for " + Name);
        return false;
    }
    return true;
}

static void GINScheckSubDirectories(const COLstring& DevDir, const COLstring& ComponentName, 
                                   	COLmap<COLstring, COLarray<COLstring>>* pMissingUpstreams) {
	COL_FUNCTION(GINScheckSubDirectories);
    COL_TRC("Checking subdirectories of: " + DevDir);
    FILdirEnumerator SubDirs(FILpathAppend(DevDir, FIL_ANYFILE), true);
    COLstring SubDirName, Error;
    while(SubDirs.getFile(SubDirName)) {
        if(SubDirName == ".git") { continue; }
        const COLstring SubDirFullPath = FILpathAppend(DevDir, SubDirName);
        const COLstring SubDirGit = FILpathAppend(SubDirFullPath, ".git");
        if (!GINScheckGitRemote(SubDirGit, SubDirName, &Error)) {
            (*pMissingUpstreams)[ComponentName].push_back(
                FILpathAppend(ComponentName + "/dev", SubDirName));
        }
    }
}

static bool GINScheckUpstreamsInDir(const COLstring& RootDir, 
                                   COLmap<COLstring, COLarray<COLstring>>* pMissingUpstreams, 
                                   COLvar* pOut) {
    COL_FUNCTION(GINScheckUpstreamsInDir);
    try {
        FILdirEnumerator Enum(FILpathAppend(RootDir, FIL_ANYFILE), true);
        COLstring FileName, Error;
        while(Enum.getFile(FileName)) {
            if(FileName == ".git") { continue; }
            const COLstring DevDir = FILpathAppend(FILpathAppend(RootDir, FileName), "dev");
            const COLstring DevGit = FILpathAppend(DevDir, ".git");
            if (!GINScheckGitRemote(DevGit, FileName, &Error)) {
                pMissingUpstreams->add(FileName, COLarray<COLstring>());
            }
            GINScheckSubDirectories(DevDir, FileName, pMissingUpstreams);
        }
        return true;
    } catch(const COLerror& E) {
        COL_TRC("Error checking library upstreams: " + E.description());
        GINSpushSetResult(pOut, false, E.description());
        return false;
    }
}

static COLstring GINSformatWarningMessage(const COLmap<COLstring, COLarray<COLstring>>& MissingUpstreams) {
	COL_FUNCTION(GINSformatWarningMessage);
	COLstring Warning = "These components and/or libraries have no upstream repository git url "
							 "and will not function properly when pulled: <ul>";

	for (const auto& it : MissingUpstreams) {
		const COLstring& Component = it.first;
		const COLarray<COLstring>& Libraries = it.second;
		Warning += "<li><a href='#dashboard/details?component=" + Component + "'>" + Component + "</a></li>";
		for (const auto& Library : Libraries) {
			Warning += "<li><a href='#translator?component=" + Component + "&development=true&edit=main.lua'>" + Library + "</a></li>";
		}
	}
	Warning += "</ul>";
	return Warning;
}


static bool GINScheckMissingUpstreams(COLvar* pOut) {
    COL_FUNCTION(GINScheckMissingUpstreams);
    COLmap<COLstring, COLarray<COLstring>> MissingUpstreams;
    if(!GINScheckUpstreamsInDir(DIRcompsDir(), &MissingUpstreams, pOut)) { return false; }
    if(MissingUpstreams.empty()) { return true; }
    (*pOut)["last_warning"] = GINSformatWarningMessage(MissingUpstreams);
    return true;
}

// Everytime we attempt a push, we update attempted timestamp
// Everytime we push successfully, we update successful timestamp and clear the last_error
// Everytime we push unsuccessfully, we update the last_error with the error message
void GINSpushWorker(const COLvar& In, COLvar* pOut) {
	COL_FUNCTION(GINSpushWorker);
	COLvar	 Config;
	COLstring Error;
	if(!GINSloadAndParse(&Config, &Error)) { return GINSpushSetResult(pOut, false, Error); }
	const COLstring User		= Config.exists("user") ? Config["user"] : "";
	if(User.is_null() || !PRMcheckRoleAccess(In["from_translator"], User, "", MANAGE_INSTANCE, &Error)) {
		(*pOut)["error"] 	 = "User " + User + " lacks the " + PRMsetError(MANAGE_INSTANCE);
		(*pOut)["success"] = false;
		return;
	}
	const COLstring Dir			= DIRconfigFile("");
	const bool		 Force		= In["force"].asBool();
	const COLstring LastWarning = In["last_warning"].asString();
	const time_t	 LastCommit = atoll(In["successful"].asString().c_str());
	const bool		 NeedsPush	= Force || GINScommitCheckInterval(LastCommit, Config["interval"].asInt());
	COL_VAR4(User, Dir, Force, NeedsPush);
	if(!NeedsPush) {
		COL_TRC("No push needed");
		(*pOut)["output"] = "No push needed";
		(*pOut)["last_warning"] = LastWarning;
		return;
	}
	COLstring Output;
	if(!GITpush(User, Dir, "", &Output, &Error)) { return GINSpushSetResult(pOut, false, Error); }
	if(!GINScheckMissingUpstreams(pOut)) { return; }
	(*pOut)["output"] = "Successfully pushed";
	(*pOut)["success"] = true;
}

// /instance/push
void GINSpush(const COLwebRequest& Request, SCKloop* pLoop, GINScommitInfo* pCommitInfo) {
   COL_FUNCTION(GINSpush);
	COLstring Error;
	bool Manual = COLvarGetBool(Request.Data, "manual", false);
	if(Manual && !PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", MANAGE_INSTANCE, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(MANAGE_INSTANCE));
   }
	COLvar Data	  = GINScommitInfoToVar(*pCommitInfo);
	Data["force"] = COLvarGet(Request.Data, "force", false);
	Data["last_warning"] 	= pCommitInfo->LastWarning;
	Data["from_translator"] = Request.FromTranslator;
	SCKinvokeWorker(pLoop, GINSpushWorker, Data, COLnewClosure1(GINSpushCallback, pCommitInfo, Request.Address));
}
