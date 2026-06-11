#ifndef _GRIDHTTP_H_
#define _GRIDHTTP_H_
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDhttp.h
//
//  Description: Initial http communication for grid
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLstring.h>

COLstring GRIDIbuildHttpUpgradeRequest(const COLstring& IguanaId, const COLstring& WebSocketKey, const COLstring& Path);
COLstring GRIDIbuildHttpUpgradeResponse(const COLstring& ClientKey, const COLstring& IguanaId);

bool GRIDIparseHttpResponse(const COLstring& Response, const COLstring& ExpectedKey);
bool GRIDIparseHttpRequest(const COLstring& Request, COLstring& OutClientKey);

#endif  // end of defensive include
