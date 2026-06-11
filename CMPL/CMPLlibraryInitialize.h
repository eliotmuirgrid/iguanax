#ifndef __CMPL_LIBRARY_INITIALIZE_H__
#define __CMPL_LIBRARY_INITIALIZE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPLlibraryInitialize
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Friday 26 May 2023 - 10:19AM
// ---------------------------------------------------------------------------

struct CFGmap;
class COLwebRequest;
class COLaddress;
class COLstring;
class GITUmodule;
class SCKloop;

void CMPLlibraryInitialize(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop);

#endif // end of defensive include
