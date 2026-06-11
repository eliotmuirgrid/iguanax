// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCgitAheadBehind
//
// Description:
//
// Implementation
//
// Author: Tyler Brown
// Date:   Friday 17 March 2023 - 10:33AM
// ---------------------------------------------------------------------------
#include <CFG/CFGconfig.h>
#include <CFG/CFGmapApi.h>
#include <CMPC/CMPCgitAheadBehind.h>
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
#include <GIT/GITpath.h>
#include <GITU/GITUaheadBehindLibrary.h>
#include <GITU/GITUmodules.h>
#include <GITU/GITUrepository.h>
#include <GITU/GITUurl.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

static bool CMPCreposSynchronized(const COLstring& Dir, const COLstring& RepoUrl) {
   COL_FUNCTION(CMPCreposSynchronized);
   COLstring UpstreamRepo = GITUrepositoryRemoteUrl(Dir);
   COL_VAR2(RepoUrl, UpstreamRepo);
   return RepoUrl == UpstreamRepo;
}

static void CMPCassignBehindAheadToMap(const COLstring& Dir, const COLstring& ModuleName, const COLstring& ComponentId,
                                       const COLstring& RepoUrl, COLvar* pOutput, COLstring* pError) {
   COL_FUNCTION(CMPCassignBehindAheadToMap);
   bool Ahead       = false;
   bool Behind      = false;
   bool CorrectRepo = false;
   bool Initialized = FILfileExists(Dir + ".git");
   if(Initialized) {
      GITUaheadBehindLibrary(Dir, &Ahead, &Behind, pError);
      CorrectRepo = CMPCreposSynchronized(Dir, RepoUrl);
   }
   COL_VAR2(Ahead, Behind);
   COLvar  Data;
   COLvar& Output       = *pOutput;
   Data["correct_repo"] = CorrectRepo;
   Data["can_push"]     = Ahead;
   Data["can_pull"]     = Behind;
   Data["initialized"]  = Initialized;
   const COLstring Key  = FILpathAppend(ComponentId, ModuleName);
   Output[Key]          = Data;
   COL_VAR(Output);
}

static void CMPCgitAheadBehindImpl(COLstring Component, const bool Development, COLaddress Address, SCKloop* pLoop,
                                   COLclosure0* pCallback) {
   COL_FUNCTION(CMPCgitAheadBehindImpl);
   COLvar    Result;
   COLstring ComponentDir = DIRcomponentProjectDir(Component, Development);
   COLstring Error, GitUrl;
   GITUrepository(ComponentDir, "origin", &GitUrl);
   COLmap<COLstring, GITUmodule> ModuleList;
   COLstring                     FileName = ComponentDir + ".gitmodules";
   if(FILfileExists(FileName)) GITUmoduleLoad(FileName, &ModuleList);
   ModuleList.add("", GITUmodule(GitUrl, ""));  // so that we check the main repo too
   GITUmoduleGetCommits(ComponentDir, "", "", &ModuleList);
   for(auto i = ModuleList.begin(); i != ModuleList.end(); ++i) {
      COLstring ModuleName = i->first;
      COL_VAR(ModuleName);
      COLstring LibraryDir = GITUmoduleDirFull(Component, ModuleName, Development);
      CMPCassignBehindAheadToMap(LibraryDir, ModuleName, Component, i->second.GitUrl, &Result, &Error);
   }
   COLrespondSuccess(Address, Result);
   pLoop->post(pCallback);
}

// /component/git_ahead_behind
void CMPCgitAheadBehind(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop) {
   COL_FUNCTION(CMPCgitAheadBehind);
   const COLvar& Data = Request.Data;
   COL_VAR(Data.json(1));
   COLvar          Result;
   const COLstring Component = COLvarRequiredString(Data, "component");
   bool            Dev       = COLvarRequiredBool(Data, "development");
   ROLid           Id        = Dev ? ROLid::EDIT_SCRIPT : ROLid::VIEW_SCRIPT;
   CFGconfig*      pConfig   = CFGmapNode(pMap, Component);
   if(!pConfig) { return COLrespondError(Request.Address, Component); }
   COLstring Error;
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, pConfig->CoreConfig.Guid, Id, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(Id));
   }
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&CMPCgitAheadBehindImpl, pConfig->CoreConfig.Guid, Dev,
                                                    Request.Address, pLoop,
                                                    CMPCcallback("/component/git_ahead_behind")));
}