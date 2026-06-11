#ifndef GINRAPI_H
#define GINRAPI_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GINRapi.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   17/12/24 10:41 AM
//  ---------------------------------------------------------------------------
#include <COL/COLweb.h>
#include <COL/COLhashmap.h>

void GINRapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap);

#endif // end of defensive include
