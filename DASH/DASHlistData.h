#ifndef DASHLISTDATA_H
#define DASHLISTDATA_H
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: DASHdashboardListData
//
// Author: Tyler Brown
//
// Returns all the relevant data for rendering the list in the dashboard.
//---------------------------------------------------------------------------
class COLwebRequest;
struct CFGmap;
class SCKloop;

void DASHlistData(const COLwebRequest& Request, CFGmap* pMap);

#endif
