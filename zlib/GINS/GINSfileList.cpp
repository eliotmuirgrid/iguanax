// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GINSfileList
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Wednesday 11 September 2024 - 08:31AM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <GINS/GINSfileList.h>
#include <GIT/GITcommitStatus.h>
#include <GITU/GITUuncommittedChanges.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

void GINSfileListCallback() {
	COL_FUNCTION(GINSfileListCallback);
	COL_TRC("GINSfileList api completed");
}

bool GINScommitFileList(const COLstring& Dir, const COLstring& CommitId, const COLstring& RelativePath, COLvar* pOut, COLstring* pError){
   COL_FUNCTION(GINScommitFileList);
   COLstring& Error = *pError;
   COLvar& Data = *pOut;
   COLstring Path = FILpathAppend(Dir, RelativePath);
   COLstring Content;
   if(!GITcommitStatus(Path, CommitId, &Content, &Error)){
      return false;
   }
   GITUparseUncommittedOutput(RelativePath, Content, false, "", &Data);
   Data = Data[RelativePath];
   return true;
}

void GINSfileListImpl(COLvar In, COLaddress Address, COLclosure0* pCallback, SCKloop* pLoop){
   COL_FUNCTION(GINSfileListImpl);
   const COLstring CommitId     = COLvarRequiredString(In, "commit");
   const COLstring RelativePath = COLvarRequiredString(In, "relative_path");
   COLstring Error;
   COLvar Data;
   COLstring Dir = DIRconfigFile("");
   if(!GINScommitFileList(Dir, CommitId, RelativePath, &Data, &Error)){
      COLrespondError(Address, Error);
      pLoop->post(pCallback);
      return;
   }
   COLvar Result;
   Result["status"] = Data;
   Result["dir"] = Dir;
   COLrespondSuccess(Address, Result);
   pLoop->post(pCallback);
}

// /instance/file_list
void GINSfileList(const COLwebRequest& Request, SCKloop* pLoop) {
   COL_FUNCTION(GINSfileList); 
   const COLvar& Data = Request.Data;
   COL_VAR(Data.json(1));
   COLstring Error;
   COL_VAR(Request.Data.json(1));
   const COLstring CommitId     = COLvarRequiredString(Data, "commit");
   const COLstring RelativePath = COLvarRequiredString(Data, "relative_path");
	COLclosure0* pCallback = COLnewClosure0(&GINSfileListCallback);
	pLoop->threadPool()->scheduleTask(COLnewClosure0(&GINSfileListImpl, Data, Request.Address, pCallback, pLoop));
}
