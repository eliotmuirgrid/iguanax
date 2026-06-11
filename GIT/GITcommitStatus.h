#ifndef __GIT_COMMIT_STATUS_H__
#define __GIT_COMMIT_STATUS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITcommitStatus
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Wednesday 31 July 2024 - 11:04AM
// ---------------------------------------------------------------------------

class COLstring;

bool GITcommitStatus(const COLstring& Dir, const COLstring& Args, COLstring* pOut, COLstring* pError);

#endif // end of defensive include
