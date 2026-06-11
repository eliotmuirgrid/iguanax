#ifndef __CMPC_GET_REPOSITORY_H__
#define __CMPC_GET_REPOSITORY_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCgetRepository
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Wednesday 22 March 2023 - 03:58PM
// ---------------------------------------------------------------------------
struct CFGmap;
class SCKloop;
class COLwebRequest;

void CMPCgetRepository(const COLwebRequest& Request, SCKloop* pLoop);

#endif // end of defensive include
