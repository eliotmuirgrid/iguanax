#ifndef __CMPC_SOURCES_ALLOWABLE_H__
#define __CMPC_SOURCES_ALLOWABLE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCsourcesAllowable
//
// Description:
//
// TODO
//
// Author: Vismay Shah 
// Date:   Wednesday 31 July 2024 - 08:36AM
// ---------------------------------------------------------------------------

struct CFGmap;
class COLwebRequest;
class SCKloop;

void CMPCsourcesAllowable(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop);

#endif // end of defensive include
