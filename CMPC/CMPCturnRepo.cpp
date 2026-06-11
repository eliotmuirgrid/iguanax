// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCturnRepo
//
// Description:
//
// Implementation
//
// Author: Matthew Sobkowski
// Date:   Monday 28 October 2024 - 05:10PM
// ---------------------------------------------------------------------------
#include <BLUA/BLUAmanager.h>
#include <CFG/CFGconfig.h>
#include <CFG/CFGmapApi.h>
#include <CMPC/CMPCturnPlain.h>
#include <CMPC/CMPCturnRepo.h>
#include <CMPC/CMPCutils.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GINS/GINSinitialize.h>
#include <GIT/GITadd.h>
#include <GIT/GITcommit.h>
#include <GIT/GITinit.h>
#include <GITU/GITUutils.h>
#include <INI/INIdata.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKloop.h>
#include <SCK/SCKworker.h>
COL_LOG_MODULE;

static void CMPCturnRepoCallback(COLvar Result, CFGmap* pMap, COLaddress Address) {
   COL_FUNCTION(CMPCturnRepoCallback);
   COL_VAR(Result.json(1));
   COLstring Component = Result["component"];
   if(!Component.is_null()) {
      CFGconfig* pConfig = CFGmapNode(pMap, Component);
      if(pConfig) pConfig->CoreConfig.Commit = "";
   }
   Result["success"].asBool() ? COLrespondSuccess(Address) : COLrespondError(Address, Result["error"]);
}

static void CMPCturnRepoErrorAndCleanup(const COLstring& Component, const COLstring& Error, COLvar* pOut) {
   const COLstring ScratchDir       = DIRscratchDir("");
   const COLstring ComponentScratch = DIRscratchDir(Component);
   COLstring       RemoveError;
   FILremoveFullDirNewSafe(ComponentScratch, &RemoveError);
   if(FILisDirectoryEmpty(ScratchDir)) FILremoveFullDirNewSafe(ScratchDir, &RemoveError);
   (*pOut)["error"]   = Error;
   (*pOut)["success"] = false;
}

static void CMPCturnRepoImpl(const COLvar& Data, COLvar* pOut) {
   COL_FUNCTION(CMPCturnRepoImpl);
   const COLstring Component           = Data["component"];
   const COLstring Username            = Data["username"];
   const COLstring CompDir             = DIRcomponentDir(Component);
   const COLstring ComponentScratch    = DIRscratchDir(Component);
   const COLstring ComponentScratchDev = FILpathAppend(ComponentScratch, "dev/");
   try {
      FILcopyDirectoryContents(CompDir, ComponentScratch, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
      const COLstring CompJson = FILpathAppend(ComponentScratch, "component.json");
      COLstring       Data;
      FILreadFile(CompJson, &Data);
      COLvar CompJsonVar;
      if(CompJsonVar.parse(Data)) {
         CompJsonVar["run_commit"] = "";
         CompJsonVar["run_comp"]   = "";
         FILwriteFile(CompJson, CompJsonVar.json(1));
      }
   } catch(const COLerror& E) { return CMPCturnRepoErrorAndCleanup(Component, E.description(), pOut); }
   COLstring Error;
   if(!GITinit(Username, ComponentScratchDev, false, &Error)) {
      COL_TRC("GITinit failed: " << Error);
      return CMPCturnRepoErrorAndCleanup(Component, Error, pOut);
   }
   if(!GITadd(ComponentScratchDev, ".", true, &Error)) {
      COL_TRC("GITadd failed: " << Error);
      return CMPCturnRepoErrorAndCleanup(Component, Error, pOut);
   }
   if(!GITcommit(Username, ComponentScratchDev, "--allow-empty", "Initialized component repository.", &Error)) {
      COL_TRC("GITcommit failed: " << Error);
      return CMPCturnRepoErrorAndCleanup(Component, Error, pOut);
   }
   if(!FILremoveFullDirNewSafe(CompDir, &Error)) {
      (*pOut)["error"]   = Error;
      (*pOut)["success"] = false;
      return;
   }
   FILcopyDirectoryContents(ComponentScratch, CompDir, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
   FILremoveFullDirNewSafe(ComponentScratch, &Error);
   if(FILisDirectoryEmpty(DIRscratchDir(""))) { FILremoveFullDirNewSafe(DIRscratchDir(""), &Error); }
   GINSaddComponent(Component);
   CMPCremoveTracking(Component);
   (*pOut)["success"]   = true;
   (*pOut)["component"] = Component;
}

// /component/turn_repo
void CMPCturnRepo(const COLwebRequest& Request, BLUAinstanceManager* pManager, CFGmap* pMap, SCKloop* pLoop) {
   COL_FUNCTION(CMPCturnRepo);
   COL_VAR(Request.Data.json(1));
   const COLstring Component = COLvarRequiredString(Request.Data, "component");
   CFGconfig*      pConfig   = CFGmapNode(pMap, Component);
   if(!pConfig) { return COLrespondError(Request.Address, Component); }
   COLstring Error;
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, pConfig->CoreConfig.Guid, ROLid::EDIT_SCRIPT, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_SCRIPT));
   }
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, pConfig->CoreConfig.Guid, ROLid::EDIT_UPSTREAM,
                          &Error)) {
      COL_TRC("You don't have permission");
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_UPSTREAM));
   }
   if(BLUAinstanceRunning(*pManager, Component)) {
      return COLrespondError(Request.Address, "You must stop the component before making modifications.");
   }
   COLvar Data;
   Data["component"] = Component;
   Data["username"]  = Request.User;
   SCKinvokeWorker(pLoop, &CMPCturnRepoImpl, Data, COLnewClosure1(&CMPCturnRepoCallback, pMap, Request.Address));
}
