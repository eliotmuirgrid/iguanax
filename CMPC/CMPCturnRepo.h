#ifndef __CMPC_TURN_REPO_H__
#define __CMPC_TURN_REPO_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCturnRepo
//
// Description:
//
// TODO
//
// Author: Matthew Sobkowski 
// Date:   Monday 28 October 2024 - 05:10PM
// ---------------------------------------------------------------------------
struct CFGmap;
class COLwebRequest;
class SCKloop;

void CMPCturnRepo(const COLwebRequest& Request, BLUAinstanceManager* pManager, CFGmap* pMap, SCKloop* pLoop);


#endif // end of defensive include
