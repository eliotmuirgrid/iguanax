// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBGget
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 22 March 2023 - 11:07AM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DBG/DBGget.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
COL_LOG_MODULE;

static void DBGgetImp(const COLstring& FileName, COLvar& Result){
   COL_FUNCTION(DBGgetImp);
   COLstring SafeName = FILpathName(FileName);
   COL_VAR2(SafeName, FileName);
   SafeName = DIRcrashDir() + SafeName;
   COLstring Content;
   FILreadFile(SafeName, &Content);
   Result["content"] = Content;
   Result["name"]    = FileName;
}

// /debug/crash/file/get
void DBGget(const COLwebRequest& Request){
   COL_FUNCTION(DBGget);
   COL_VAR(Request.Data);
   COLstring Error;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::CRASH_FILE, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::CRASH_FILE));
   }

   COLvar Result;
   DBGgetImp(COLvarRequiredString(Request.Data, "file"), Result);
   COLrespondSuccess(Request.Address, Result);
}
