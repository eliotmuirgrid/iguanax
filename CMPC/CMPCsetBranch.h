#ifndef MAIN2PROJECT_CMPCSETBRANCH_H
#define MAIN2PROJECT_CMPCSETBRANCH_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: CMPCsetBranch.h
//
//  Description: API for creating and checking out git branches in components.
//  Supports both creating new branches and switching to existing branches
//  with proper validation and error handling.
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
class COLwebRequest;
struct CFGmap;
class SCKloop;

void CMPCsetBranch(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop);

#endif  // end of defensive include
