//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: CMPCarchiveRemove.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   10/04/25 10:49 AM
//  ---------------------------------------------------------------------------
#include <CMPC/CMPCarchiveRemove.h>
#include <COL/COLlog.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <PRM/PRMcallbacks.h>
#include <SCK/SCKworker.h>
#include <SDB/SDBcapi.h>
COL_LOG_MODULE;

static void CMPCarchiveRemoveCallback(COLvar Result, COLstring User, COLaddress Address) {
   COL_FUNCTION(CMPCarchiveRemoveCallback);
   COL_VAR(Result.json(1));
   if(!Result["success"].asBool()) { return COLrespondError(Address, Result["error"].asString()); }
   SDBIlogService("Archived components removed by " + User);
   COLrespondSuccess(Address);
}

static void CMPCarchiveRemoveImpl(const COLvar& In, COLvar* pOut) {
   COL_FUNCTION(CMPCarchiveRemoveImpl);
   const COLstring ArchivedDir = DIRarchiveDir();
   COLstring       Err;
   if(FILfileExists(ArchivedDir)) { FILremoveFullDirNewSafe(ArchivedDir, &Err); }
   (*pOut)["success"] = Err.is_null();
   (*pOut)["error"]   = Err;
}

// /component/archive_remove
void CMPCarchiveRemove(const COLwebRequest& Request, SCKloop* pLoop) {
   COL_FUNCTION(CMPCarchiveRemove);
   COLstring Err;
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", CLEAR_ARCHIVE, &Err)) {
      return COLrespondError(Request.Address, PRMsetError(CLEAR_ARCHIVE));
   }
   const COLvar Data;
   SCKinvokeWorker(pLoop, CMPCarchiveRemoveImpl, Data,
                   COLnewClosure1(CMPCarchiveRemoveCallback, Request.User, Request.Address));
}
