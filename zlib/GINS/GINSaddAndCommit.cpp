//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GINSaddAndCommit.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   07/10/24 1:36 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <GINS/GINSaddAndCommit.h>
#include <GIT/GITadd.h>
#include <GIT/GITcommit.h>
#include <SCK/SCKworker.h>
COL_LOG_MODULE;

static void GINSaddAndCommitCallback(COLvar Result, COLaddress Address) {
	COL_FUNCTION(GINSaddAndCommitCallback);
	Result["success"].asBool() ? COLrespondSuccess(Address) : COLrespondError(Address, Result["error"]);
}

static void GINSaddComponent(const COLstring& Component, bool All, COLstring* pErr) {
	COL_FUNCTION(GINSaddComponent);
	COLstring CompsDir = DIRcompsDir();
	COLstring Path = Component + "/";
	if(!All) {
		Path += "component.json";
	}
	FILcorrectPathSeparators(Path);
	GITadd(CompsDir, Path, true, pErr);
}

static void GINSaddComponents(const COLvar& Components, const COLstring& Component, bool All, COLstring* pErr) {
	COL_FUNCTION(GINSaddComponents);
	COLstring CompsDir = DIRcompsDir();
   for (int i = 0; i < Components.size(); i++){
      const COLstring& ComponentId = Components[i];
		GINSaddComponent(ComponentId, All, pErr);
	}
	if(!Component.is_null()) {
		GINSaddComponent(Component, All, pErr);
	}
	COLstring ConfigDir = DIRconfigDir();
	GITadd(ConfigDir, ".gitmodules", true, pErr);
}

void GINSaddSettings(COLstring* pErr){
	COL_FUNCTION(GINSaddSettings);
	COL_TRC("Adding settings");
	COLstring SettingsDir = DIRsettingsDir();
	GITadd(SettingsDir, ".", true, pErr);
}

void GINSaddReports(COLstring* pErr){
	COL_FUNCTION(GINSaddReports);
	COL_TRC("Adding reports");
	COLstring ReportsDir = DIRcrashDir();
	GITadd(ReportsDir, ".", true, pErr);
}

void GINSaddUsers(COLstring* pErr){
	COL_FUNCTION(GINSaddUsers);
	COL_TRC("Adding users");
	COLstring UsersDir = DIRusersDir();
	GITadd(UsersDir, ".", true, pErr);
}

void GINSaddOther(COLstring* pErr){
	COL_FUNCTION(GINSaddOther);
	COL_TRC("Adding other");
	COLstring OtherDir = DIRotherDir();
	GITadd(OtherDir, ".", true, pErr);
}

bool GINSaddAndCommitImpl(COLvar In, COLstring* pErr) {
	COL_FUNCTION(GINSaddAndCommitImpl);
	COL_VAR(In.json(1));
	const COLstring& Username = In["user"];
	const bool& AllowEmpty	  = In["allow_empty"];
	const COLvar& Components  = In["components"];
	const COLstring& Component = In["component"];
	const bool& Settings      = In["settings"];
	const bool& Users         = In["users"];
	const bool& Reports       = In["reports"];
	const bool& Other         = In["other"];
	const bool& All           = In["all"];
	COLstring Message  = In["message"];
	Message.replace("\"", "\\\"");
	if(Components.size() > 0 || !Component.is_null()) {
		COL_TRC("Adding components");
		GINSaddComponents(Components, Component, All, pErr);
	}
	if(Settings) GINSaddSettings(pErr);
	if(Reports)  GINSaddReports(pErr);
	if(Users)    GINSaddUsers(pErr);
	if(Other)    GINSaddOther(pErr);
	const COLstring Flag = AllowEmpty ? "--allow-empty" : "";
	const COLstring Dir = DIRconfigDir();
	return GITcommit(Username, Dir, Flag, Message, pErr);
}

static void GINSaddAndCommitWorker(const COLvar& In, COLvar* pOut) {
	COL_FUNCTION(GINSaddAndCommitWorker);
	COLstring Error;
	const bool Success = GINSaddAndCommitImpl(In, &Error);
	COL_VAR2(Success, Error);
	(*pOut)["success"] = Success;
	if(!Success) { (*pOut)["error"] = Error; }
}

// instance/add_and_commit
void GINSaddAndCommit(const COLwebRequest& Request, SCKloop* pLoop) {
	COL_FUNCTION(GINSaddAndCommit);
	COL_VAR(Request.Data.json(1));
	COLvar Data;
	Data["message"]	  = COLvarRequiredString(Request.Data, "message");
	Data["allow_empty"] = COLvarGetBool(Request.Data, "allow_empty", false);
	Data["settings"]    = COLvarGetBool(Request.Data, "settings", false);
	Data["users"]       = COLvarGetBool(Request.Data, "users", false);
	Data["reports"]     = COLvarGetBool(Request.Data, "reports", false);
	Data["all"]         = COLvarGetBool(Request.Data, "all", false);
	Data["other"]       = COLvarGetBool(Request.Data, "other", false);
	Data["components"]  = COLvarGetObject(Request.Data, "components", "");
	Data["component"]   = COLvarGetString(Request.Data, "component", "");
	Data["user"]		  = Request.User;
	SCKinvokeWorker(pLoop, GINSaddAndCommitWorker, Data, COLnewClosure1(GINSaddAndCommitCallback, Request.Address));
}