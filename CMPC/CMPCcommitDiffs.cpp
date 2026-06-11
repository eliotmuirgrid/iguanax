// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCcommitDiffs
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Thursday 26 September 2024 - 04:39PM
// ---------------------------------------------------------------------------

#include <CMPC/CMPCcommitDiffs.h>
#include <CFG/CFGmap.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <GINS/GINSdiff.h>
#include <GINS/GINSfileList.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

void CMPCcommitDiffsCallback(){
   COL_FUNCTION(CMPCcommitDiffsCallback);
   COL_TRC("CMPCcommitDiffs api completed");
}

void CMPCcommitDiffsImpl(COLvar Data, COLaddress Address, COLclosure0* pCallback, SCKloop* pLoop){
   COL_FUNCTION(CMPCcommitDiffsImpl);
   COLstring ComponentId, Error;
   const COLstring CommitId = COLvarRequiredString(Data, "commit");
   const COLstring RelativePath = COLvarRequiredString(Data, "relative_path");
   COLvar FileList;
   if (!CFGcomponentId(Data, &ComponentId, &Error)){
      return COLrespondError(Address, Error);
   }
   COLstring Dir = DIRcomponentDevDir(ComponentId);
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
// /component/commit_diffs
void CMPCcommitDiffs(const COLwebRequest& Request, SCKloop* pLoop) {
   COL_FUNCTION(CMPCcommitDiffs);
   const COLvar& Data = Request.Data;
   COL_VAR(Data);
   const COLstring Component = COLvarRequiredString(Data, "component");
   const COLstring CommitId = COLvarRequiredString(Data, "commit");
   const COLstring RelativePath = COLvarRequiredString(Data, "relative_path");
	COLclosure0* pCallback = COLnewClosure0(&CMPCcommitDiffsCallback);
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&CMPCcommitDiffsImpl, Data, Request.Address, pCallback, pLoop));
}
