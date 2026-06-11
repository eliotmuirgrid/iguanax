// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCcurrentCommitTranslator
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Monday 27 March 2023 - 02:55PM
// ---------------------------------------------------------------------------
#include <CFG/CFGconfig.h>
#include <CFG/CFGconfigApi.h>
#include <CFG/CFGmapApi.h>
#include <CFG/CFGmap.h>
#include <CMPC/CMPCcurrentCommitTranslator.h>
#include <CMPC/CMPCutils.h>
#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <GITU/GITUcommitInfo.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

void CMPCcurrentCommitTranslatorImp(COLstring Component, const bool Dev, COLaddress Address, SCKloop* pLoop,
												COLclosure0* pCallback){
   COL_FUNCTION(CMPCcurrentCommitTranslatorImp);
	COLvar Result;
   COLstring RepoDir;
   RepoDir = Dev ? DIRcomponentDevDir(Component) : DIRcomponentRunDir(Component);
   COL_VAR(RepoDir);
   GITUcommitInfo(RepoDir, &Result);
   COL_VAR(Result.json(1));
	COLrespondSuccess(Address, Result);
	pLoop->post(pCallback);
}

// /component/ide/commit/info
void CMPCcurrentCommitTranslator(const COLwebRequest& Request, SCKloop* pLoop) {
   COL_FUNCTION(CMPCcurrentCommitTranslator);
	COLstring ComponentId, Error;
	bool Dev						  = COLvarRequiredBool(Request.Data, "development");
	if (!CFGcomponentId(Request.Data, &ComponentId, &Error)){
      return COLrespondError(Request.Address, Error);
   }
	pLoop->threadPool()->scheduleTask(COLnewClosure0(&CMPCcurrentCommitTranslatorImp, ComponentId, Dev,
																	 Request.Address, pLoop,
																	 CMPCcallback("/component/ide/commit/info")));
} 
