// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SECupdate
//
// Author: Matthew Sobkowski
// Date: 2024-11-28
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLweb.h>
#include <SCK/SCKloop.h>
#include <SCK/SCKworker.h>
#include <SEC/SECaccess.h>
#include <SEC/SECupdate.h>
#include <ROL/ROLid.h>
#include <PRM/PRMcallbacks.h>
COL_LOG_MODULE;

void SECupdateCallback(COLvar Result, COLaddress Address) {
   COL_FUNCTION(SECupdateCallback);
   COL_VAR(Result.json(1));
   if(!Result["success"].asBool()) { return COLrespondError(Address, Result["error"]); }
   SECset(Result["data"]);
   COLrespondSuccess(Address);
}

void SECupdateWorker(const COLvar& In, COLvar* pOut) {
   COL_FUNCTION(SECupdateWorker);
   COL_VAR(In);
   COLstring Error;
   if(!SECsave(In, &Error)) {
      (*pOut)["success"] = false;
      (*pOut)["error"]   = Error;
      return;
   }
   COL_TRC("Allowed dirs saved");
   (*pOut)["data"]    = In;
   (*pOut)["success"] = true;
}

// /security/set
void SECupdate(const COLwebRequest& Request, SCKloop* pLoop) {
   COL_FUNCTION(SECupdate);
   COLstring Error;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", EDIT_SECURITY, &Error)){
      return COLrespondError(Request.Address, PRMsetError(EDIT_SECURITY));
   }
   SCKinvokeWorker(pLoop, SECupdateWorker, Request.Data, COLnewClosure1(SECupdateCallback, Request.Address));
}
