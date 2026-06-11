#ifndef __CMPL_LIBRARY_REMOTE_REMOVE_H__
#define __CMPL_LIBRARY_REMOTE_REMOVE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPLlibraryRemoteRemove
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Thursday 06 April 2023 - 05:08PM
// ---------------------------------------------------------------------------

struct CFGmap;
class COLwebRequest;
class SCKloop;

void CMPLlibraryRemoteRemove(const COLwebRequest& Request, SCKloop* pLoop);
void CMPLlibraryRemoveUpstreamHelper(const COLstring& Component, const COLstring& LibraryName);

#endif // end of defensive include
