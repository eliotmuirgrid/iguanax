#ifndef __CMPL_LIBRARY_INITIALIZE_ALL_H__
#define __CMPL_LIBRARY_INITIALIZE_ALL_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPLlibraryInitializeAll
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Friday 26 May 2023 - 01:49PM
// ---------------------------------------------------------------------------

struct CFGmap;
class COLwebRequest;
class SCKloop;

void CMPLlibraryInitializeAll(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop);

#endif // end of defensive include
