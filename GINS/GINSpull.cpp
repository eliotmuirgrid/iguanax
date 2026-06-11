// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GINSpull
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Wednesday 11 September 2024 - 01:10PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <GINS/GINSpull.h>
#include <GINS/GINSutils.h>
#include <GIT/GITpull.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

void GINSpullCallback() {
	COL_FUNCTION(GINSpullCallback);
	COL_TRC("GINSpull api completed");
}

void GINSpullImpl(COLvar In, COLaddress Address, COLclosure0* pCallback, SCKloop* pLoop) {
   COL_FUNCTION(GINSpullImpl);
   COLvar Config, Out;
   COLstring Error;
	if(!GINSloadAndParse(&Config, &Error)) {
		COLrespondError(Address, Error);
		return pLoop->post(pCallback);
	}
	const COLstring Username = Config.exists("user") ? Config["user"] : "";
	const COLstring Dir		 = DIRconfigFile("");
   COL_VAR(Dir);
   COLstring Output;
   if(!GITpull("", Dir, &Output, &Error)){
      COLrespondError(Address, Error);
      return pLoop->post(pCallback);
   }
   Out["output"] = Output;
	COLrespondSuccess(Address, Out);
   pLoop->post(pCallback);
}

void GINSpull(const COLwebRequest& Request, SCKloop* pLoop) {
   COL_FUNCTION(GINSpull);
   const COLvar& Data = Request.Data;
   COLclosure0* pCallback = COLnewClosure0(&GINSpullCallback);
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&GINSpullImpl, Data, Request.Address, pCallback, pLoop));
}
