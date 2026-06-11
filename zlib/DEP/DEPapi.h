#ifndef __DEP_API_H__
#define __DEP_API_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DEPapi
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Wednesday 16 April 2025 - 06:50AM
// ---------------------------------------------------------------------------
#include <COL/COLweb.h>
#include <COL/COLhashmap.h>

void DEPapiWorker(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap);


#endif // end of defensive include
