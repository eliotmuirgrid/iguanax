//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: USRdelete
//
// Description:
//
// API for deleting a user
//
// Author: Matthew Sobkowski
// Date:   Mon 29 Mar 2021 11:13:50 EDT
//---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKloop.h>
#include <USR/USRdelete.h>
#include <USR/USRmap.h>
COL_LOG_MODULE;

static void USRdeleteOnWorker( COLstring Username, COLaddress Address){
   COL_FUNCTION(USRdeleteOnWorker);
   COLstring DirName = DIRconfigUserDir(Username);
   COL_TRC("Deleting directory " + DirName);
   COLstring ErrorMessage;
   bool Success = FILremoveFullDirNewSafe(DirName, &ErrorMessage);
   COL_VAR(Success);
   if (Success) { COLrespondSuccess(Address);             }
   else         { COLrespondError(Address, ErrorMessage); }
}

static void USRdeleteImp(const COLaddress& Address, COLmap<COLstring, USRuser>*  pUserMap, const COLstring& ActiveUser, const COLstring& Username, SCKloop* pLoop){
   COL_FUNCTION(USRdeleteImp);
   if (pUserMap->count(Username) == 0) { return COLrespondError(Address, "User does not exist"); }
   if (Username == ActiveUser)         { return COLrespondError(Address, "Cannot delete your own account"); }
   if ((*pUserMap)[Username].isExternalUser()) { return COLrespondError(Address, "Cannot delete external user account"); }
   pUserMap->erase(Username);
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&USRdeleteOnWorker, Username, Address));
   COL_TRC("Launched delete");
}

// /user/delete
void USRdelete(const COLwebRequest& Request, COLmap<COLstring, USRuser>* pUserMap, SCKloop* pLoop){
   COL_FUNCTION(USRdelete);
   COLstring Error;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::EDIT_USERS, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_USERS));
   }
   COL_VAR(Request.Data.json(1));
   USRdeleteImp(Request.Address, pUserMap, Request.User, COLvarRequiredString(Request.Data, "username"), pLoop);
}