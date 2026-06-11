// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCgetRepository
//
// Description:
//
// Implementation
//
// Author: Tyler Brown
// Date:   Wednesday 22 March 2023 - 03:58PM
// ---------------------------------------------------------------------------
#include <CFG/CFGconfig.h>
#include <CFG/CFGmapApi.h>
#include <CFG/CFGmap.h>
#include <CMPC/CMPCgetRepository.h>
#include <CMPC/CMPCutils.h>
#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GIT/GITbranch.h>
#include <GIT/GITutils.h>
#include <GITU/GITUmodules.h>
#include <GITU/GITUrepository.h>
#include <GSV/GSVlink.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

static void CMPCgetRepositoryLinks(const COLstring& UpstreamRepo, const COLstring& Branch, COLvar& Result) {
   COL_FUNCTION(CMPCgetRepositoryLinks);
   COL_VAR(UpstreamRepo);
   GSVlink    Link;
   const bool Success = GSVconstructLink(UpstreamRepo, &Link, Branch);
   COLvar     Links   = GSVlinkToVar(Link);
   if(Success) {
      Result["link"] = Links["web"];
      Result["urls"] = Links;
   } else {
      Result["error"] = "Error creating link for " + UpstreamRepo;
   }
   COL_VAR(Result.json(1));
}

static COLstring CMPCgetLibRepo(const COLstring& CompRepo, const COLstring& Dir, const COLstring& Module) {
   COL_FUNCTION(CMPCgetLibRepo);
   COL_VAR3(CompRepo, Dir, Module);
   const COLstring ModulesFIle = FILpathAppend(Dir, ".gitmodules");
   if(!FILfileExists(ModulesFIle)) { return CompRepo; }
   COLmap<COLstring, GITUmodule> Modules;
   GITUmoduleLoad(ModulesFIle, &Modules);
   for(const auto& it : Modules) {
      if(it.first == Module) {
         COL_VAR2(it.first, it.second.GitUrl);
         return it.second.GitUrl;
      }
   }
   return CompRepo;
}

static void CMPCgetRepositoryImpl(COLstring Component, COLstring Path, bool Development, COLaddress Address,
                                  COLclosure0* pCallback, SCKloop* pLoop) {
   COL_FUNCTION(CMPCgetRepositoryImpl);
   COL_VAR2(Component, Path);
   const COLstring RepoDir = DIRcomponentProjectDir(Component, Development);
   const COLstring Dir     = FILpathAppend(RepoDir, Path);
   COLstring Branch  = GITbranchCurrentDev(Dir);
   const COLstring DefaultBranch  = GITrepositoryDefaultBranch(Dir);
   COLstring       UpstreamRepo;
   COLvar          Result;
   if(!Development) {
      Branch = CMPCgetRunBranch(Component);
      UpstreamRepo = CMPCgetRunRepo(Component);
      if(!Path.is_null()) { UpstreamRepo = CMPCgetLibRepo(UpstreamRepo, RepoDir, Path); }
      Result["repo"] = UpstreamRepo;
      CMPCgetRepositoryLinks(UpstreamRepo, Branch, Result);
   } else if(GITisPlainFolder(Component)) {
      Result["plain"] = true;
   } else if(GITUrepository(Dir, "origin", &UpstreamRepo)) {
      Result["repo"] = UpstreamRepo;
      CMPCgetRepositoryLinks(UpstreamRepo, Branch, Result);
   }
   Result["branch"] = Branch;
   Result["default_branch"] = DefaultBranch;
   COLrespondSuccess(Address, Result);
   pLoop->post(pCallback);
}

// /component/get_repository
void CMPCgetRepository(const COLwebRequest& Request, SCKloop* pLoop) {
   COL_FUNCTION(CMPCgetRepository);
   COL_VAR(Request.Data.json(1));
   COLstring ComponentId, Error;
   const COLstring Path        = COLvarGetString(Request.Data, "path", "");
   const bool      Development = COLvarRequiredBool(Request.Data, "development");
   if (!CFGcomponentId(Request.Data, &ComponentId, &Error)){
      return COLrespondError(Request.Address, Error);
   }
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&CMPCgetRepositoryImpl, ComponentId, Path, Development,
                                                    Request.Address, CMPCcallback("component/get_repository"), pLoop));
}
