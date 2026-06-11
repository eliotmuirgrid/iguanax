// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCcommits
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Tuesday 28 February 2023 - 10:20AM
// ---------------------------------------------------------------------------
#include <CFG/CFGconfig.h>
#include <CFG/CFGconfigApi.h>
#include <CFG/CFGconfigSave.h>
#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>
#include <CMPC/CMPCcommits.h>
#include <CMPC/CMPCutils.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILatomic.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GIT/GITcommits.h>
#include <GIT/GITpath.h>
#include <GIT/GITutils.h>
#include <GITU/GITUrepository.h>
#include <SCK/SCKworker.h>
COL_LOG_MODULE;

void CMPCcreationComplete(COLvar Result, IWEBconfig* pWebConfig, COLaddress Address) {
   COL_FUNCTION(CMPCcreationComplete);
   COL_VAR(Result);
   return COLrespondSuccess(Address, Result);
}

static void CMPCcommitsImpl(const COLvar& In, COLvar* pOut){
   COL_FUNCTION(CMPCcommitsImpl);
	COLstring ComponentId = In["component"];
	COLstring RepoDir     = DIRcomponentDevDir(ComponentId);
	COLstring AllCommits, RelCommits,Error;
   bool PlainFolder = GITisPlainFolder(ComponentId);
   COL_VAR(PlainFolder);
   if(!PlainFolder && GITcommits(RepoDir, &AllCommits, &Error, true, true)){
      GITparseCommits(AllCommits, &(*pOut));
   }

   if(PlainFolder && GITcommits(RepoDir, &RelCommits, &Error, false, true)){
      GITparseCommits(RelCommits, &(*pOut));
   }

   if(!Error.is_null()){
      COL_TRC("Error getting commits: " << Error);
      (*pOut)["error"] = Error;
   }
}

// "/component/commits"
void CMPCcommits(const COLwebRequest& Request, IWEBconfig* pWebConfig, SCKloop* pLoop){
   COL_FUNCTION(CMPCcommits);
   const COLvar& Data = Request.Data;
   COL_VAR(Data.json(1));
   COLstring ComponentId, Error;
   if (!CFGcomponentId(Data, &ComponentId, &Error)){
      return COLrespondError(Request.Address, Error);
   }
	COLvar NewData;
	NewData["component"] = ComponentId;
   COL_VAR(NewData.json(1));
   SCKinvokeWorker(pLoop, CMPCcommitsImpl, NewData, COLnewClosure1(CMPCcreationComplete, pWebConfig, Request.Address));
}
