#ifndef __CMPC_BORDER_SET_H__
#define __CMPC_BORDER_SET_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCborderSet
//
// Description:
//
// Api to set the component status border. Called using SCKcall by BLUA
// when we do component.setBorder in the lua script.
//
// Author: Vismay Shah 
// Date:   Friday 19 May 2023 - 1:00PM
// ---------------------------------------------------------------------------
class COLwebRequest;
struct CFGmap;

void CMPCborderSet(const COLwebRequest& Request, CFGmap* pMap);

#endif // end of defensive include
