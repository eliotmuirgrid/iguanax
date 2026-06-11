// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBGlist
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
#include <COL/COLweb.h>
#include <CRH/CRHupload.h>
#include <DBG/DBGlist.h>
#include <DIR/DIRpath.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>
COL_LOG_MODULE;

// /debug/crash/file/list
void DBGlist(const COLwebRequest& Request){
   COL_FUNCTION(DBGlist);
   COL_VAR(Request.Data);
   COLvar Result;
   Result["last_upload"] = FILpathName(CRHlastUpload());
   Result["crash_list"].setArrayType();
   FILdirEnumerator Dirs(DIRcrashDir() + "*.crash",false, true);
   int Count = 0;
   COLstring FileName;
   while (Dirs.getFile(FileName)){
      Result["crash_list"][Count] = FileName;
      Count++;
      COL_VAR2(Count, FileName);
   }
   COLrespondSuccess(Request.Address, Result);
}
