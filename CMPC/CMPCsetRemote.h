#ifndef __CMPC_SET_REMOTE_H__
#define __CMPC_SET_REMOTE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCsetRemote
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Thursday 23 March 2023 - 12:16PM
// ---------------------------------------------------------------------------
class COLwebRequest;
struct CFGmap;
class SCKloop;

void CMPCsetRemote(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop);

#endif // end of defensive include
