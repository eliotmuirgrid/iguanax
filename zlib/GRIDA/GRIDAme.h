#ifndef __GRID_API_SELF_H__
#define __GRID_API_SELF_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2026 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GRIDapiSelf
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Friday 23 January 2026 - 09:59AM
// ---------------------------------------------------------------------------
struct CFGmap;
class COLwebRequest;
class GRIDnode;
class SCKloop;

void GRIDAme(const COLwebRequest& Request, GRIDnode* pNode, CFGmap* pComponentMap);


#endif // end of defensive include
