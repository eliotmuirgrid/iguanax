#ifndef __GITC_CHECK_REPO_PATH_H__
#define __GITC_CHECK_REPO_PATH_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITCcheckRepoPath
//
// Description:
//
// Api to check if a repository path is correct. For BitBucket, this is the 
// form <workspace>/<repository>. For GitLab, this could be either
// <username>/<project> or <groupname>/<project>.
//
// Author: Vismay Shah 
// Date:   Tuesday 09 April 2024 - 04:21PM
// ---------------------------------------------------------------------------
class COLwebRequest;

void GITCcheckRepoPath(const COLwebRequest& Request);

#endif // end of defensive include
