#ifndef _GRIDAPIGETSTATS_H
#define _GRIDAPIGETSTATS_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDapiGetStats.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
class COLwebRequest;
class GRIDnode;
class SCKloop;

void GRIDAgetCurrentStats(const COLwebRequest& Request, GRIDnode* pNode, SCKloop* pLoop);
void GRIDAgetStatHistory(const COLwebRequest& Request, GRIDnode* pNode, SCKloop* pLoop);

#endif  // end of defensive include
