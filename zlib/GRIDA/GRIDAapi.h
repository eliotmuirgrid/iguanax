#ifndef __GRID_API_H__
#define __GRID_API_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDapi
//
//  Description: API endpoints for Active-Passive failover cluster management
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLhashmap.h>
#include <COL/COLweb.h>

struct CFGmap;
class GRIDnode;
class SCKloop;

void GRIDAapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap, SCKloop* pLoop,GRIDnode* pNode, CFGmap* pComponentMap);

#endif
