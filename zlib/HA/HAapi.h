#ifndef MAIN2PROJECT_HAAPI_H
#define MAIN2PROJECT_HAAPI_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: HAapi.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLweb.h>
#include <COL/COLhashmap.h>

void HAapi(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap);

#endif  // end of defensive include
