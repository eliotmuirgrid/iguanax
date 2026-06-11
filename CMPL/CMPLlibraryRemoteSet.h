#ifndef __CMPL_LIBRARY_REMOTE_SET_H__
#define __CMPL_LIBRARY_REMOTE_SET_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPLlibraryRemoteSet
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Thursday 06 April 2023 - 05:19PM
// ---------------------------------------------------------------------------

struct CFGmap;
class COLstring;
class COLwebRequest;
class SCKloop;

void CMPLlibraryRemoteSet(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop);


#endif // end of defensive include
