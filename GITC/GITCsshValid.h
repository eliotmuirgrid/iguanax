#ifndef __GITC_SSH_VALID_H__
#define __GITC_SSH_VALID_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITCsshValid
//
// Description:
//
// Api to check if a ssh key works with a git cloud host
//
// Author: Vismay Shah 
// Date:   Friday 05 April 2024 - 02:28PM
// ---------------------------------------------------------------------------
class COLwebRequest;
class SCKloop;
class GSVconfig;

struct GSVitem;

void GITCsshValid(const COLwebRequest& Request, bool* pGitCacheRefreshing, SCKloop* pLoop);
bool GITCsshCheckAccess(const COLstring& IguanaUser, const GSVconfig& Config, const GSVitem& Item,
								COLstring* pError);  // exposed for unit tests

#endif // end of defensive include
