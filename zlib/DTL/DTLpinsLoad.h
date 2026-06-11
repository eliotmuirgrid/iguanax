#ifndef __DTLPINSLOAD_H__
#define __DTLPINSLOAD_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: DTLpositionLoad.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   16/07/24 3:30 PM
//  ---------------------------------------------------------------------------
#include <COL/COLmap.h>
#include <COL/COLweb.h>

struct CFGmap;
class SCKloop;
struct DTLpinsPos
{
   int PosX;
   int PosY;
};
struct DTLpinsData{
   COLmap<COLstring, DTLpinsPos> Map;
};

bool DTLpinsParse(const COLvar& Pins, CFGmap* pMap, DTLpinsData* pData, COLstring* pError);
void DTLpinsToVar(const DTLpinsData& Data, COLvar* pPins);

bool DTLpinsLoadFile(const COLstring& Username, COLvar* pPins, COLstring* pError);
bool DTLpinsLoadImpl(const COLstring& User, CFGmap* pMap, COLvar* pPins, COLstring* pError);
void DTLpinsLoad(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop);

#endif  // end of defensive include
