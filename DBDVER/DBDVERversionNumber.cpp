//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDVERversionNumber
//
// Description:
//
// Implementation
//
// Author: Nicolas Lehman
// Date:   Mon Mar 15 2021
//---------------------------------------------------------------------------

#include <DBDVER/DBDVERversion.h>
#include <DBDVER/DBDVERgitInfo.h>
#include <DBDVER/DBDVERtime.h>

#include <FIL/FILutils.h>

#include <COL/COLweb.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// /product/version
void DBDVERgetVersionNumber (const COLwebRequest& Request) {
   COL_FUNCTION(DBDVERgetVersionNumber);
   COLvar Result;
   Result["version"]        = DBD_STRING_VERSION;
   Result["branch"]         = DBDVER_BRANCH_NAME;
   Result["timestamp"]      = DBDVERconvertEpoch();
   Result["commit_id"]      = DBDVER_COMMIT_ID;
   Result["commit_message"] = DBDVER_COMMIT_MSG;
   Result["executable"]     = FILexecutableDir() + FILexecutableFileName();
   Result["working_dir"]    = FILworkingDir();
   Result["user"]           = FILcurrentUser();
   COL_VAR(Result);
   COLrespondSuccess(Request.Address, Result);
}