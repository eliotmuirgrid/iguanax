#ifndef __DTLPINSSAVE_H__
#define __DTLPINSSAVE_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: DTLpinsSave.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   16/07/24 3:29 PM
//  ---------------------------------------------------------------------------
#include <COL/COLweb.h>
struct CFGmap;
class SCKloop;

bool DTLpinsSaveFile(const COLstring& User, const DTLpinsData& Data, COLstring* pError);
void DTLpinsSave(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop);

#endif  // end of defensive include
