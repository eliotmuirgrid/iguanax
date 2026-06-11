#ifndef __CMPC_MAKE_REMOTE_SSH_H__
#define __CMPC_MAKE_REMOTE_SSH_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCmakeRemoteSsh
//
// Description:
//
// Change the remote Url from HTTPS to SSH so we can change it.
//
// Author: Eliot Muir 
// Date:   Friday 29 September 2023 - 04:47PM
// ---------------------------------------------------------------------------
struct CFGmap;
class COLwebRequest;
class COLstring;
class SCKloop;

void CMPCmakeEditable(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop);

#endif // end of defensive include
