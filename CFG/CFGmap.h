#ifndef __CFG_MAP_H__
#define __CFG_MAP_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CFGmap
//
// Description: Storage struct for components
//
// Author: Shuo Hou
// Date:   Tuesday, Aug 31th, 2021
//---------------------------------------------------------------------------
#include "CFGconfig.h"
#include <COL/COLmap.h>

//using CFGmap = COLmap<COLstring, CFGconfig>; // potential idea rather than struct?
struct CFGmap {
   COLmap<COLstring, CFGconfig> ConfigMap;
   bool HasInvalid = false;
};

// This is to allow worker thread apis to resolve a component name --> component guid
// without having to convert them to main thread apis.
void CFGmapLookupSet(CFGmap* pComponentMap);
const CFGmap& CFGmapGet();

COLostream& operator<<(COLostream& Stream, const CFGmap& Map);
void CFGconvertNameToIdIfNeeded(const COLvar& NameArray, CFGmap* pMap, COLarray<COLstring>* pComponentGuids, COLarray<COLstring>* pBadComponents);
bool CFGcomponentId(const COLvar& Data, COLstring* pComponentId, COLstring* pErr);

#endif // end of defensive include
