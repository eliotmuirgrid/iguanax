#ifndef __EXPT_API_H__
#define __EXPT_API_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: EXPTapi
//
// Description:
//
// Api for exporting translator files and sample data
//
// Author: Vismay Shah 
// Date:   Friday 14 November 2025 - 12:43PM
// ---------------------------------------------------------------------------
#include <COL/COLhashmap.h>
#include <COL/COLweb.h>

void EXPTapi(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap);

#endif // end of defensive include
