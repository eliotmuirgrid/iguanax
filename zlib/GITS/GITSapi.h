#ifndef __GITS_GITSAPI_H__
#define __GITS_GITSAPI_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GITSapi.h
//
//  Description: Api for installing git in safe mode
//
//  Author: Matthew Sobkowski
//  Date:   4/27/23 1:38 PM
//  ---------------------------------------------------------------------------
#include <COL/COLhashmap.h>
#include <COL/COLweb.h>

void GITSapi(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap);

#endif // end of defensive include
