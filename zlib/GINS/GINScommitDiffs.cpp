// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GINScommitDiffs
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Tuesday 24 September 2024 - 10:55AM
// ---------------------------------------------------------------------------

#include <GINS/GINScommitDiffs.h>

#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <GINS/GINSfileList.h>
#include <GINS/GINSdiff.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;


void GINScommitDiffsCallback(){
   COL_FUNCTION(GINScommitDiffsCallback);
   COL_TRC("GINScommitDiffs api completed");
}

void GINScommitDiffsImpl(COLvar Data, COLaddress Address, COLclosure0* pCallback, SCKloop* pLoop){
   COL_FUNCTION(GINScommitDiffsImpl);
   const COLstring CommitId     = COLvarRequiredString(Data, "commit");
   const COLstring RelativePath = COLvarRequiredString(Data, "relative_path");
   COLstring Error;
   COLvar FileList;
   COLstring Dir = DIRconfigFile("");
   if(!GINScommitFileList(Dir, CommitId, RelativePath, &FileList, &Error)){
      COLrespondError(Address, Error);
      pLoop->post(pCallback);
      return;
   }
   COL_VAR(FileList.json(1));
   COLvar Result;
   COLstring Path = FILpathAppend(Dir, RelativePath);
   for(int i = 0; i < FileList.size(); i++){
      const COLstring& FileName = FileList[i]["name"];
      COL_VAR(FileName);
      GINSgetFileDiffs(Path, CommitId, FileName, &Result[FileName]);
   }
   COL_VAR(Result.json(1));
   COLrespondSuccess(Address, Result);
   pLoop->post(pCallback);
}

// /instance/commit_diffs
void GINScommitDiffs(const COLwebRequest& Request, SCKloop* pLoop) {
   COL_FUNCTION(GINScommitDiffs); 
   const COLvar& Data = Request.Data;
   COL_VAR(Data.json(1));
   COLstring Error;
   COL_VAR(Request.Data.json(1));
   const COLstring CommitId     = COLvarRequiredString(Data, "commit");
   const COLstring RelativePath = COLvarRequiredString(Data, "relative_path");
	COLclosure0* pCallback = COLnewClosure0(&GINScommitDiffsCallback);
	pLoop->threadPool()->scheduleTask(COLnewClosure0(&GINScommitDiffsImpl, Data, Request.Address, pCallback, pLoop));
}
