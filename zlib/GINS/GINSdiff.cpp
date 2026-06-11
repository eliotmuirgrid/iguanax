// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GINSdiff
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Wednesday 11 September 2024 - 07:55AM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GINS/GINSdiff.h>
#include <HTOOL/HTOOLfileDiff.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

void GINSdiffCallback() {
	COL_FUNCTION(GINSdiffCallback);
	COL_TRC("GINSdiff api completed");
}

void GINSgetFileDiffs(COLstring Path, COLstring CommitId, COLstring FileName, COLvar* pOutput){
   COL_FUNCTION(GINSgetFileDiffs);
   COLvar& Output = *pOutput;
   COLstring FilePath    = FILpathAppend(Path, FileName);
   bool      FileExists  = FILfileExists(FilePath);
   COL_VAR3(FilePath, FileExists, Path);
   if(!HTOOLcheckDiffParameters(FileName, FilePath, FileExists, &Output)) { 
      return;
   }
   HTOOLgetDiffContent(Path, CommitId, FileName, &Output);
}

void GINSdiffImpl(COLvar In, COLaddress Address, COLclosure0* pCallback, SCKloop* pLoop){
   COL_FUNCTION(GINSdiffImpl);
   COLvar Result;
   const COLvar FileNameArray   = COLvarRequiredObject(In, "file_name");
   const COLstring CommitId     = COLvarGetString     (In, "commit", "");
   const COLstring RelativePath = COLvarRequiredString(In, "relative_path");
   COLstring Dir = DIRconfigFile("");
   COLstring Path = FILpathAppend(Dir, RelativePath);
   for (int i = 0; i < FileNameArray.size(); i++) {
      const COLstring& FileName = FileNameArray[i];
      COLvar& Output = Result[FileName];
      GINSgetFileDiffs(Path, CommitId, FileName, &Output);
   }
   COLrespondSuccess(Address, Result);
	pLoop->post(pCallback);
}

// /instance/diff
void GINSdiff(const COLwebRequest& Request, SCKloop* pLoop) {
   COL_FUNCTION(GINSdiff);
   const COLvar& Data = Request.Data;
   COL_VAR(Data.json(1));
   COLstring Error;
   COL_VAR(Request.Data.json(1));
   const COLvar FileNameArray   = COLvarRequiredObject(Data, "file_name");
   const COLstring CommitId     = COLvarGetString     (Data, "commit", "");
   const COLstring RelativePath = COLvarGetString     (Data, "relative_path", "");
	COLclosure0* pCallback = COLnewClosure0(&GINSdiffCallback);
	pLoop->threadPool()->scheduleTask(COLnewClosure0(&GINSdiffImpl, Data, Request.Address, pCallback, pLoop));
}
