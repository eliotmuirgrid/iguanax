#ifndef __GIT_INIT_H__
#define __GIT_INIT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITinit
//
// Description:
//
// Initialize a repo
//
// Author: Tyler Brown 
// Date:   Wednesday 05 April 2023 - 05:30PM
// ---------------------------------------------------------------------------

class COLstring;

bool GITinit(const COLstring& Username, const COLstring& Dir, bool InitialCommit, COLstring* pError);
bool GITinitConfig(const COLstring& Username, const COLstring& NewRepoLocation, bool InitialCommit, COLstring* pError);
bool GITinitEmptyCommit(const COLstring& Username, const COLstring& Dir, COLstring* pError);

#endif // end of defensive include
