#ifndef __CFG_MAP_API_H__
#define __CFG_MAP_API_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CFGmapApi
//
// Description:
//
// Functional routines to work on CFGmap data-structure.
//
// Author: Eliot Muir
// Date:   Wed 13 Jul 2022 15:11:43 EDT
//---------------------------------------------------------------------------
class COLstring;
class COLvar;
struct CFGmap;
struct CFGconfig;

// Will remove orphaned links - don't use before adding all the components
void CFGremoveDeletedSources(CFGmap* pMap);

// Does not check for duplicates.
void CFGmapAddComponent(CFGmap* pMap, const CFGconfig& Config);

// checks for component json on disk
bool CFGmapAddComponentFromGuid(CFGmap* pMap, const COLstring& Component, const COLstring& ServerGuid);

bool CFGmapremoveComponent(CFGmap* pMap, const COLstring& ComponentGuid);

// Returns nullptr if guid not found
CFGconfig* CFGmapNode(CFGmap* pMap, const COLstring& Component);

bool CFGmapNameExists(CFGmap* pMap, const COLstring& Name, CFGconfig* pComponent);

// Check if any components except self have given name
bool CFGmapNameExists(CFGmap* pMap, const COLstring& Guid, const COLstring& Name);

void CFGmapNames(CFGmap* pMap, COLvar* pOutput);

void CFGmapRemoveNodesFromServer(CFGmap* pMap, const COLstring& ServerGuid);

// return true of ComponentId is a part of a cycle in pMap
bool CFGmapHasComponentCycle(CFGmap* pMap, const COLstring& ComponentId);

#endif // end of defensive include
