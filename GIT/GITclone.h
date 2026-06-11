#ifndef __GIT_CLONE_H__
#define __GIT_CLONE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITclone
//
// Description:
//
// Simple clone operation
//
// Author: Eliot Muir
// Date:   Monday 13 February 2023 - 09:53AM
// ---------------------------------------------------------------------------
#include <COL/COLmap.h>

class COLstring;
class GITcloneState;
class GITUlist;

class COLthreadPool;
class SCKloop;

// if UseSSH is true, the repo with be cloned via ssh
// if UseSSH is false, HTTPS will be attempted before SSH for the cloning. If SSH ended up to be used, UseSSH is set to true.
bool GITinitializeCachedRepo(const COLstring& Username, const COLstring& Url, const COLstring& Branch,
                             const COLstring& NewRepoLocation, bool& UseSsh, COLstring* pOut, COLstring* pError);

// Refreshes a cached repository in place by force-aligning the current branch with its upstream.
bool GITrefreshCachedRepo(const COLstring& Username, const COLstring& Url, const COLstring& Branch,
                          const COLstring& RepoLocation, bool& UseSsh, COLstring* pOut, COLstring* pError);
// Clones remote repository into local cache.
bool GITclone(const COLstring& Username, const COLstring& GitSourceLocation, const COLstring& NewRepoLocation,
              COLstring* pOut, COLstring* pError);

#endif  // end of defensive include
