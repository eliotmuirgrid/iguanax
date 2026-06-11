#ifndef _GRIDINIT_H
#define _GRIDINIT_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDinit
//
//  Description: Initialization and configuration management for GRID nodes
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
class COLstring;
class GRIDnode;
class IWEBconfig;
class SCKloop;

bool GRIDIinit(const COLstring& IguanaId, IWEBconfig* pWebConfig, SCKloop* pLoop, GRIDnode** ppNode);
void GRIDIreInit(GRIDnode* pNode);

#endif
