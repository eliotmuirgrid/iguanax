#ifndef __CMPC_STATUS_SET_H__
#define __CMPC_STATUS_SET_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCstatusSet
//
// Description:
//
// Api to set the component status html block. Called using SCKcall by BLUA
// when we do component.setStatus in the lua script.
//
// Author: Vismay Shah 
// Date:   Friday 19 May 2023 - 10:00AM
// ---------------------------------------------------------------------------
class COLwebRequest;
struct CFGmap;

void CMPCstatusSet(const COLwebRequest& Request, CFGmap* pMap);

#endif // end of defensive include
