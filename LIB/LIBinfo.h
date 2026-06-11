#ifndef LIBINFO_H
#define LIBINFO_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LIBinfo.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   09/06/25 2:34 PM
//  ---------------------------------------------------------------------------
#include <COL/COLweb.h>

struct CFGmap;

void LIBinfo(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop);

#endif // end of defensive include
