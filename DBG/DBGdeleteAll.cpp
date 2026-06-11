// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBGdeleteAll
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 22 March 2023 - 04:16PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <DBG/DBGdeleteAll.h>
#include <DIR/DIRpath.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILutils.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
COL_LOG_MODULE;

// /debug/crash/file/delete/all
void DBGdeleteAll(const COLwebRequest& Request){
   COL_FUNCTION(DBGdeleteAll);
   COL_VAR(Request.Data);
   COLstring Error;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::CRASH_FILE, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::CRASH_FILE));
   }
   COLvar Result;
	const COLstring CrashDir = DIRcrashDir();
	FILdirEnumerator Dirs(CrashDir + "*.crash",false, true);
	int Count = 0;
	COLstring FileName;
	while (Dirs.getFile(FileName)){
		Count++;
		COL_VAR2(Count, FileName);
		COLstring FullFileName = CrashDir + FileName;
		FILremoveWithThrow(FullFileName);
	}
   COLrespondSuccess(Request.Address);
}
