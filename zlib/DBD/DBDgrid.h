#ifndef _DBDGRID_H
#define _DBDGRID_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: DBDgrid.h
//
//  Description: Hooks and init functions for GRID system
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
class WEBserver;
class IWEBconfig;
class GRIDnode;
class SCKloop;

void DBDgridInit(WEBserver* pWebServer, IWEBconfig* pWebConfig, GRIDnode** ppGridNode, SCKloop* pLoop);

#endif  // end of defensive include
