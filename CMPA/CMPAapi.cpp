// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCapi
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday 15 February 2023 - 10:29AM
// ---------------------------------------------------------------------------
#include <CFG/CFGmap.h>
#include <CMPA/CMPAapi.h>
#include <CMPC/CMPCarchiveList.h>
#include <CMPC/CMPCarchiveRemove.h>
#include <CMPC/CMPCborderSet.h>
#include <CMPC/CMPCcommitDiffs.h>
#include <CMPC/CMPCcommits.h>
#include <CMPC/CMPCcreate.h>
#include <CMPC/CMPCcurrentCommitTranslator.h>
#include <CMPC/CMPCdestinationsAllowable.h>
#include <CMPC/CMPCfetchReposThreaded.h>
#include <CMPC/CMPCfieldsGet.h>
#include <CMPC/CMPCfieldsSet.h>
#include <CMPC/CMPCgetRepository.h>
#include <CMPC/CMPCgitAheadBehind.h>
#include <CMPC/CMPCgitHistory.h>
#include <CMPC/CMPCmakeEditable.h>
#include <CMPC/CMPCmergesPending.h>
#include <CMPC/CMPCpromoteChanges.h>
#include <CMPC/CMPCremoveRemote.h>
#include <CMPC/CMPCsetBranch.h>
#include <CMPC/CMPCsetCommit.h>
#include <CMPC/CMPCsetEditable.h>
#include <CMPC/CMPCsetRemote.h>
#include <CMPC/CMPCsourcesAllowable.h>
#include <CMPC/CMPCstatusSet.h>
#include <CMPC/CMPCtooltipSet.h>
#include <CMPC/CMPCturnPlain.h>
#include <CMPC/CMPCturnRepo.h>
#include <CMPL/CMPLaddLibrary.h>
#include <CMPL/CMPLlibraryCreate.h>
#include <CMPL/CMPLlibraryDelete.h>
#include <CMPL/CMPLlibraryInitialize.h>
#include <CMPL/CMPLlibraryInitializeAll.h>
#include <CMPL/CMPLlibraryList.h>
#include <CMPL/CMPLlibraryRemoteRemove.h>
#include <CMPL/CMPLlibraryRemoteSet.h>
#include <CMPL/CMPLlibraryRename.h>
#include <CMPL/CMPLlibraryRenameContent.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void CMPAapiMain(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap, SCKloop* pLoop,
                 IWEBconfig* pWebConfig, BLUAinstanceManager* pManager, CFGmap* pMap, bool* pGitCacheRefreshing) {
   COL_FUNCTION(CMPAapiMain);
   // COMPONENT API'S
   ApiMap.add("/component/commits", COLnewClosure1(&CMPCcommits, pWebConfig, pLoop));
   ApiMap.add("/component/create", COLnewClosure1(&CMPCcreate, pWebConfig, pMap, pManager, pLoop));
   ApiMap.add("/component/status/set", COLnewClosure1(&CMPCstatusSet, pMap));
   ApiMap.add("/component/tooltip/set", COLnewClosure1(&CMPCtooltipSet, pMap));
   ApiMap.add("/component/border/set", COLnewClosure1(&CMPCborderSet, pMap));
   //ApiMap.add("/component/set_editable", COLnewClosure1(&CMPCsetEditable, pMap, pLoop));
   ApiMap.add("/component/fetch_repositories",
              COLnewClosure1(&CMPCfetchRepositoriesThreaded, pGitCacheRefreshing, pLoop));

   ApiMap.add("/component/get_repository", COLnewClosure1(&CMPCgetRepository, pLoop));
   ApiMap.add("/component/ide/commit/info", COLnewClosure1(&CMPCcurrentCommitTranslator, pLoop));
   ApiMap.add("/component/set_commit", COLnewClosure1(&CMPCsetCommit, pMap, pLoop));

   ApiMap.add("/component/git_ahead_behind", COLnewClosure1(&CMPCgitAheadBehind, pMap, pLoop));
   ApiMap.add("/component/remove_upstream", COLnewClosure1(&CMPCremoveRemote, pLoop));

   ApiMap.add("/component/fields/get", COLnewClosure1(&CMPCfieldsGet, pMap, pManager, pLoop));
   ApiMap.add("/component/fields/set", COLnewClosure1(&CMPCfieldsSet, pMap, pLoop));

   ApiMap.add("/component/make_editable", COLnewClosure1(&CMPCmakeEditable, pMap, pLoop));

   ApiMap.add("/component/sources/allowable", COLnewClosure1(&CMPCsourcesAllowable, pMap, pLoop));
   ApiMap.add("/component/destinations/allowable", COLnewClosure1(&CMPCdestinationsAllowable, pMap, pLoop));

   ApiMap.add("/component/set_upstream", COLnewClosure1(&CMPCsetRemote, pMap, pLoop));
   ApiMap.add("/component/set_branch", COLnewClosure1(&CMPCsetBranch, pMap, pLoop));
   ApiMap.add("/component/promote_changes", COLnewClosure1(&CMPCpromoteChanges, pMap, pLoop));
   ApiMap.add("/component/promotion_state", COLnewClosure1(&CMPCpromotionState, pMap, pLoop));
   ApiMap.add("/component/turn_plain", COLnewClosure1(&CMPCturnPlain, pManager, pMap, pLoop));

   ApiMap.add("/component/git_history", COLnewClosure1(&CMPCgitHistory, pWebConfig, pLoop));
   ApiMap.add("/component/commit_diffs", COLnewClosure1(&CMPCcommitDiffs, pLoop));

   ApiMap.add("/component/merge_conflicts", COLnewClosure1(&CMPCmergesPending, pMap, pLoop));
   ApiMap.add("/component/turn_repo", COLnewClosure1(&CMPCturnRepo, pManager, pMap, pLoop));

   ApiMap.add("/component/archive_list", COLnewClosure1(&CMPCarchiveList, pLoop));
   ApiMap.add("/component/archive_remove", COLnewClosure1(&CMPCarchiveRemove, pLoop));

   // LIBRARY API'S
   ApiMap.add("/component/library/add", COLnewClosure1(&CMPLaddLibrary, pLoop));
   ApiMap.add("/component/library/delete", COLnewClosure1(&CMPLlibraryDelete, pLoop));
   ApiMap.add("/component/library/create", COLnewClosure1(&CMPLlibraryCreate, pLoop));
   ApiMap.add("/component/library/list", COLnewClosure1(&CMPLlibraryList, pLoop));
   ApiMap.add("/component/library/initialize", COLnewClosure1(&CMPLlibraryInitialize, pMap, pLoop));
   ApiMap.add("/component/library/initialize/all", COLnewClosure1(&CMPLlibraryInitializeAll, pMap, pLoop));
   ApiMap.add("/component/library/set_upstream", COLnewClosure1(&CMPLlibraryRemoteSet, pMap, pLoop));
   ApiMap.add("/component/library/remote/remove", COLnewClosure1(&CMPLlibraryRemoteRemove, pLoop));
   ApiMap.add("/component/library/rename", COLnewClosure1(&CMPLlibraryRename, pLoop));
   ApiMap.add("/component/library/rename/content", COLnewClosure1(&CMPLlibraryRenameContent, pLoop));
}
