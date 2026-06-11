#ifndef GINIAPI_H
#define GINIAPI_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GINIapi.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   21/05/25 11:58 AM
//  ---------------------------------------------------------------------------
#include <COL/COLhashmap.h>
#include <COL/COLweb.h>

void GINIapi(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap);

#endif // end of defensive include
