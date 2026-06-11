#ifndef __HTOOL_HTOOLAPI_H__
#define __HTOOL_HTOOLAPI_H__
// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: HTOOLapi.h
//
//  Description
//
//  Author: Matthew Sobkowski
//  Date:   10/19/22 1:10 PM
// ---------------------------------------------------------------------------
#include <COL/COLweb.h>
#include <COL/COLhashmap.h>

void HTOOLapiAsync(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap);

#endif // end of defensive include
