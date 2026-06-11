//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GINSremoveRemote
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   20/02/25 10:27 AM
//  ---------------------------------------------------------------------------
#include <COL/COLclosure.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <GINS/GINSremoveUpstream.h>
#include <GINS/GINSutils.h>
#include <GITU/GITUremoveRemote.h>
#include <PRM/PRMcallbacks.h>
#include <SCK/SCKloop.h>
#include <SCK/SCKworker.h>
COL_LOG_MODULE;

static void GINSremoveUpstreamCallback(COLvar Result, COLaddress Address) {
   COL_FUNCTION(GINSremoveUpstreamCallback);
   Result["success"].asBool() ? COLrespondSuccess(Address, Result)
                              : COLrespondError(Address, "No remote was confiugred for this instance");
}

static void GINSremoveUpstreamWorker(const COLvar& In, COLvar* pOut) {
   COL_FUNCTION(GINSremoveUpstream);
   const COLstring ConfigDir = DIRconfigDir();
   const COLstring OldRemote = GITUremoveRemote(ConfigDir);
   (*pOut)["old_remote"]     = OldRemote;
   (*pOut)["success"]        = !OldRemote.is_null();
}

// /instance/remove_upstream
void GINSremoveUpstream(const COLwebRequest& Request, SCKloop* pLoop) {
   COL_FUNCTION(GINSremoveUpstream);
   COLstring Err;
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::MANAGE_INSTANCE, &Err)) {
      return COLrespondError(Request.Address, PRMsetError(ROLid::MANAGE_INSTANCE));
   }
   SCKinvokeWorker(pLoop, GINSremoveUpstreamWorker, Request.Data,
                   COLnewClosure1(GINSremoveUpstreamCallback, Request.Address));
}