// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GINShistory
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Tuesday 10 September 2024 - 10:42AM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <GINS/GINShistory.h>
#include <GITU/GITUfullCommitHistory.h>
#include <PRM/PRMcallbacks.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

void GINShistoryCallback() {
	COL_FUNCTION(GINShistoryCallback);
	COL_TRC("GINShistory api completed");
}

void GINShistoryImpl(COLvar In, COLaddress Address, COLclosure0* pCallback, SCKloop* pLoop){
   COL_FUNCTION(GINShistoryImpl);
   COLvar Out;
   COLstring Error;
   COLstring Dir = DIRconfigFile("");
   COL_VAR(Dir);
   COLvar& Result = Out["commits"];
   Out["path"] = Dir;
   GITUfullCommitHistory(Dir, true,&Result, &Error);
	COLrespondSuccess(Address, Out);
	pLoop->post(pCallback);
}

// /instance/git_history
void GINShistory(const COLwebRequest& Request, SCKloop* pLoop) {
   COL_FUNCTION(GINShistory);
   COL_VAR(Request.Data.json(1));
   COLstring Error;
	if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::MANAGE_INSTANCE, &Error)){
		return COLrespondError(Request.Address, PRMsetError(ROLid::MANAGE_INSTANCE));
	}
	COLclosure0* pCallback = COLnewClosure0(&GINShistoryCallback);
	pLoop->threadPool()->scheduleTask(COLnewClosure0(&GINShistoryImpl, Request.Data, Request.Address, pCallback, pLoop));
}
