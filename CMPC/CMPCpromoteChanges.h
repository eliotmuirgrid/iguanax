#ifndef __CMPC_PROMOTE_CHANGES_H__
#define __CMPC_PROMOTE_CHANGES_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2026 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: CMPCpromoteChanges.h
//
//  Description: Promote component changes between Git branches.
//  ---------------------------------------------------------------------------

class CFGmap;
class COLwebRequest;
class SCKloop;

void CMPCpromoteChanges(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop);
void CMPCpromotionState(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop);

#endif // end of defensive include
