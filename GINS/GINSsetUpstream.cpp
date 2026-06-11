// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GINSsetUpstream
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Tuesday 10 September 2024 - 07:18AM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <GINS/GINSsetUpstream.h>
#include <GITU/GITUaddRemote.h>
#include <GIT/GITpush.h>
#include <GITU/GITUrepository.h>
#include <GITU/GITUurl.h>
#include <GSV/GSVlink.h>
#include <PRM/PRMcallbacks.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

void GINSsetUpstreamCallback() {
	COL_FUNCTION(GINSsetUpstreamCallback);
	COL_TRC("GINSsetUpstream api completed");
}

bool GINSsetUpstreamCall(const COLstring& Dir, const COLstring& GitUrl, COLstring* pSshUrl, COLstring* pError) {
   COL_FUNCTION(GINSsetUpstreamCall);
	COLstring& SshUrl = *pSshUrl;
   if(!GITUsshUrl(GitUrl, &SshUrl)) {
      *pError = "Failed to parse url into ssh form";
      return false;
   }
   COL_VAR(SshUrl);
	COLstring Error;
	if(!GITUaddRemote(Dir, SshUrl, &Error)) {
		*pError = "Could not set remote repository: " + Error;
		COL_TRC(Error);
		return false;
	}
   return true;
}

void GINSsetUpstreamImpl(COLvar Data, COLaddress Address, COLclosure0* pCallback, SCKloop* pLoop) {
   COL_FUNCTION(GINSsetUpstreamImpl);
   const COLstring Username  = Data["username"];
	const COLstring GitUrl    = Data["url"];
	bool Push			        = Data["push"];
   COLstring Dir = DIRconfigFile("");
   COL_VAR(Dir);
   COLstring SshUrl;
	COLstring Error;
	if(!GINSsetUpstreamCall(Dir,GitUrl, &SshUrl, &Error)) {
		COLrespondError(Address, Error);
		return pLoop->post(pCallback);
	}
	COL_VAR(SshUrl);
   COLvar Result;
	Result["git"] = GitUrl;
	COLstring Out;
	if(Push && !GITpush(Username, Dir, "", &Out, &Error)) {
		COL_VAR2(Out, Error);
		COLrespondError(Address, Error);
	} else {
		COLrespondSuccess(Address, Result);
	}
	pLoop->post(pCallback);
}

// /instance/set_upstream
void GINSsetUpstream(const COLwebRequest& Request, SCKloop* pLoop) {
   COL_FUNCTION(GINSsetUpstream);
   COL_VAR(Request.Data.json(1));
   COLstring GitUrl      = COLvarRequiredString(Request.Data, "git");
   bool      Push        = COLvarGetBool(Request.Data, "push", false);
   COLstring Error;
   // TODO - Modify this to check for the correct role
   // if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::EDIT_UPSTREAM, &Error)){
   //    return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_UPSTREAM));
   // }
	COLvar Data;
	Data["username"]	= Request.User;
	Data["url"]			= GitUrl;
	Data["push"]		= Push;
   COL_VAR(Request.Data.json(1));
	COLclosure0* pCallback = COLnewClosure0(&GINSsetUpstreamCallback);
	pLoop->threadPool()->scheduleTask(COLnewClosure0(&GINSsetUpstreamImpl, Data, Request.Address, pCallback, pLoop));
}
