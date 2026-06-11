#ifndef __CMPC_FETCH_REPOS_THREADED_H__
#define __CMPC_FETCH_REPOS_THREADED_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCfetchReposThreaded
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Monday 20 March 2023 - 12:52PM
// ---------------------------------------------------------------------------

class COLwebRequest;
class SCKloop;

void CMPCfetchRepositoriesThreaded(const COLwebRequest& Request, bool* pGitCacheRefreshing, SCKloop* pLoop);

#endif // end of defensive include
