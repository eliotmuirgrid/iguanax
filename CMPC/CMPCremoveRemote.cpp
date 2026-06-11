// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCremoveRemote
//
// Description:
//
// Implementation
//
// Author: Tyler Brown
// Date:   Wednesday 22 March 2023 - 07:25PM
// ---------------------------------------------------------------------------
#include <CFG/CFGconfig.h>
#include <CFG/CFGmapApi.h>
#include <CFG/CFGmap.h>
#include <CMPC/CMPCremoveRemote.h>
#include <CMPC/CMPCutils.h>
#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <GINS/GINSinitialize.h>
#include <GIT/GITpull.h>
#include <GITU/GITUremoveRemote.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

static void CMPCremoveRemoteImpl(COLstring Username, COLstring Component, COLaddress Address, SCKloop* pLoop,
                                 COLclosure0* pCallback) {
   COL_FUNCTION(CMPCremoveRemoteImpl);
   COLstring Out, Err;
   const COLstring DevDir = DIRcomponentDevDir(Component);
   if(!GITpull(Username, DevDir, &Out, &Err)) {
      COL_TRC("Unable to pull: " << Err);
   }
   const COLstring OldRemote = GITUremoveRemote(DevDir);
   GINSaddComponent(Component);
   COL_VAR2(Component, OldRemote);
   COLvar Result;
   Result["old_git"] = OldRemote;
   COL_VAR(Result.json());
   COLrespondSuccess(Address, Result);
   pLoop->post(pCallback);
}

// /component/remove_upstream
void CMPCremoveRemote(const COLwebRequest& Request, SCKloop* pLoop) {
   COL_FUNCTION(CMPCremoveRemote);
   COL_VAR(Request.Data);
   COLstring ComponentId, Error;
   if (!CFGcomponentId(Request.Data, &ComponentId, &Error)){
      return COLrespondError(Request.Address, Error);
   }
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, ComponentId, ROLid::EDIT_UPSTREAM,
                          &Error)) {
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_UPSTREAM));
   }
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&CMPCremoveRemoteImpl, Request.User, ComponentId, Request.Address,
                                                    pLoop, CMPCcallback("/component/remove_upstream")));
}
