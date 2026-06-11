// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GINSapi
//
// Description:
//
// Implementation
//
// Author: Tyler Brown
// Date:   Monday 09 September 2024 - 11:34AM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <GINS/GINSaddAndCommit.h>
#include <GINS/GINSapi.h>
#include <GINS/GINSauto.h>
#include <GINS/GINSautoSet.h>
#include <GINS/GINScommitAll.h>
#include <GINS/GINScommitDiffs.h>
#include <GINS/GINSdiff.h>
#include <GINS/GINSfileList.h>
#include <GINS/GINShistory.h>
#include <GINS/GINSlastPush.h>
#include <GINS/GINSpull.h>
#include <GINS/GINSpush.h>
#include <GINS/GINSreinitialize.h>
#include <GINS/GINSremoveUpstream.h>
#include <GINS/GINSrepo.h>
#include <GINS/GINSreset.h>
#include <GINS/GINSrollback.h>
#include <GINS/GINSsetUpstream.h>
#include <GINS/GINSuser.h>
#include <GINS/GINSuserSet.h>
#include <USR/USRuser.h>
COL_LOG_MODULE;

void GINSapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap, SCKloop* pLoop,
             COLmap<COLstring, USRuser>* pUserMap, GINScommitInfo* pCommitInfo) {
   COL_FUNCTION(GINSapi);
   ApiMap.add("/instance/repo", COLnewClosure1(&GINSrepo, pLoop));
   ApiMap.add("/instance/set_upstream", COLnewClosure1(&GINSsetUpstream, pLoop));
   ApiMap.add("/instance/git_history", COLnewClosure1(&GINShistory, pLoop));
   ApiMap.add("/instance/diff", COLnewClosure1(&GINSdiff, pLoop));
   ApiMap.add("/instance/file_list", COLnewClosure1(&GINSfileList, pLoop));
   ApiMap.add("/instance/commit_diffs", COLnewClosure1(&GINScommitDiffs, pLoop));
   ApiMap.add("/instance/push", COLnewClosure1(&GINSpush, pLoop, pCommitInfo));
   ApiMap.add("/instance/pull", COLnewClosure1(&GINSpull, pLoop));
   ApiMap.add("/instance/user", COLnewClosure1(&GINSuser, pLoop, pUserMap));
   ApiMap.add("/instance/user/set", COLnewClosure1(&GINSuserSet, pLoop));
   ApiMap.add("/instance/interval", COLnewClosure1(&GINSauto, pLoop));
   ApiMap.add("/instance/interval/set", COLnewClosure1(&GINSautoSet, pLoop));
   ApiMap.add("/instance/reset", COLnewClosure1(&GINSreset, pUserMap, pLoop));
   ApiMap.add("/instance/reset/check", COLnewClosure1(&GINSresetCheck));
   ApiMap.add("/instance/last_push", COLnewClosure1(&GINSlastPush, pCommitInfo));
   ApiMap.add("/instance/rollback", COLnewClosure1(&GINSrollback, pLoop));
   ApiMap.add("/instance/commit/all", COLnewClosure1(&GINScommitAll, pLoop));
   ApiMap.add("/instance/add_and_commit", COLnewClosure1(&GINSaddAndCommit, pLoop));
   ApiMap.add("/instance/reinitialize", COLnewClosure1(&GINSreinitialize, pLoop));
   ApiMap.add("/instance/remove_upstream", COLnewClosure1(&GINSremoveUpstream, pLoop));
}
