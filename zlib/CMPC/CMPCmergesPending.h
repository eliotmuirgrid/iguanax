#ifndef __CMPC_MERGES_PENDING_H__
#define __CMPC_MERGES_PENDING_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCmergesPending
//
// Description:
//
// Scans a component directory and returns a list of files which have git merge 
// conflicts. Limited to files smaller than 1mb.
//
// Author: Vismay Shah 
// Date:   Thursday 22 August 2024 - 12:27PM
// ---------------------------------------------------------------------------
class COLwebRequest;
class SCKloop;
struct CFGmap;

void CMPCmergesPending(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop);

#endif // end of defensive include
