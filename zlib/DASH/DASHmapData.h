#ifndef DASHMAPDATA_H
#define DASHMAPDATA_H
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: DASHdashboardMapData
//
// Author: Tyler Brown
//
// Returns all the relevant data for rendering the map in the dashboard.
//---------------------------------------------------------------------------
class COLwebRequest;
struct CFGmap;
class SCKloop;

void DASHmapData(const COLwebRequest& Request, CFGmap* pMap);
void DASHmapDataImp(const COLstring& GlobTerms, bool ExactMatch, const COLarray<COLstring>& Selected, bool OnlyChecked,
                    CFGmap* pMap, COLvar& Result);

#endif
