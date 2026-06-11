// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GINSrepo
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Monday 09 September 2024 - 11:35AM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <GINS/GINSrepo.h>
#include <GITU/GITUrepository.h>
#include <GSV/GSVlink.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

void GINSrepoCallback() {
	COL_FUNCTION(GINSrepoCallback);
	COL_TRC("GINSrepo api completed");
}

void GINSrepoImpl(COLaddress Address, COLclosure0* pCallback, SCKloop* pLoop) {
   COL_FUNCTION(GINSrepoImpl);
   COLstring Dir = DIRconfigFile("");
   COL_VAR(Dir);
   COLstring UpstreamRepo;
   COLvar Result;
   if(GITUrepository(Dir, "origin", &UpstreamRepo)){
      Result["repo"] = UpstreamRepo;
      GSVlink Link;
      bool Success = GSVconstructLink(UpstreamRepo, &Link);
      COLvar Links = GSVlinkToVar(Link);
      if(Success) {
         Result["link"] = Links["web"];
         Result["urls"] = Links;
      }
      else { Result["error"] = "Error creating link for " + UpstreamRepo; }
   }
   else{
      COLrespondError(Address, "Could not determine upstream repository for " + Dir);
      return pLoop->post(pCallback);
   }
   COLrespondSuccess(Address, Result);
   pLoop->post(pCallback);
}

// /instance/repo
void GINSrepo(const COLwebRequest& Request, SCKloop* pLoop) {
   COL_FUNCTION(GINSrepo);
   COL_VAR(Request.Data.json(1));
   COLclosure0* pCallback = COLnewClosure0(&GINSrepoCallback);
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&GINSrepoImpl, Request.Address, pCallback, pLoop));
}
