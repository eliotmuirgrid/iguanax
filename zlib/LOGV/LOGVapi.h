#ifndef LOGVAPI_H
#define LOGVAPI_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LOGVapi.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   23/04/25 10:49 AM
//  ---------------------------------------------------------------------------
#include <COL/COLweb.h>
#include <COL/COLhashmap.h>

void LOGVapi(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap);

#endif // end of defensive include
