#ifndef LOGRAPI_H
#define LOGRAPI_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LOGRapi.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   16/07/25 11:12 AM
//  ---------------------------------------------------------------------------
#include <COL/COLhashmap.h>
#include <COL/COLweb.h>

void LOGRapi(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap);


#endif // end of defensive include
