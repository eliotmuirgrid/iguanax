#ifndef __CMPC_REMOVE_REMOTE_H__
#define __CMPC_REMOVE_REMOTE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCremoveRemote
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Wednesday 22 March 2023 - 07:25PM
// ---------------------------------------------------------------------------

struct CFGmap;
class COLwebRequest;
class SCKloop;

void CMPCremoveRemote(const COLwebRequest& Request, SCKloop* pLoop);

#endif // end of defensive include
