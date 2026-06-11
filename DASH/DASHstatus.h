#ifndef DASHSTATUS_H
#define DASHSTATUS_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: DASHstatus.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   19/06/25 2:19 PM
//  ---------------------------------------------------------------------------
struct CFGmap;
class COLwebRequest;
class SCKloop;

void DASHstatus(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop);

#endif // end of defensive include
