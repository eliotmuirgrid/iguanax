#ifndef __CMPC_GIT_AHEAD_BEHIND_H__
#define __CMPC_GIT_AHEAD_BEHIND_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCgitAheadBehind
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Friday 17 March 2023 - 10:33AM
// ---------------------------------------------------------------------------
struct CFGmap;
class COLwebRequest;
class SCKloop;

void CMPCgitAheadBehind(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop);


#endif // end of defensive include
