#ifndef __CMPC_SET_COMMIT_H__
#define __CMPC_SET_COMMIT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCsetCommit
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Wednesday 15 March 2023 - 01:54PM
// ---------------------------------------------------------------------------

struct CFGmap;
class COLwebRequest;
class SCKloop;

void CMPCsetCommit(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop);

#endif // end of defensive include
