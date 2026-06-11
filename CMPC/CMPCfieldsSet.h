#ifndef __CMPC_CMPCFIELDSSET_H__
#define __CMPC_CMPCFIELDSSET_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: CMPCfieldsSet.h
//
//  Description: Web api to set a field in component card
//
//
//  Author: Matthew Sobkowski
//  Date:   4/19/23 2:07 PM
//  ---------------------------------------------------------------------------
struct CFGmap;
class COLwebRequest;
class SCKloop;

void CMPCfieldsSet(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop);

#endif // end of defensive include
