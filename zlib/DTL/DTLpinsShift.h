#ifndef __DTL_PINS_SHIFT_H__
#define __DTL_PINS_SHIFT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DTLpinsShift
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Thursday 18 July 2024 - 10:14AM
// ---------------------------------------------------------------------------
#include <COL/COLweb.h>

struct CFGmap;
class SCKloop;

void DTLpinsShift(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop);

#endif // end of defensive include
