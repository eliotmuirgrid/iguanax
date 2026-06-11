#ifndef __GITU_FULL_COMMIT_HISTORY_H__
#define __GITU_FULL_COMMIT_HISTORY_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUfullCommitHistory
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Monday 29 July 2024 - 02:21PM
// ---------------------------------------------------------------------------
class COLstring;
class COLvar;

bool GITUfullCommitHistory(const COLstring& Dir, bool Development, COLvar* pResult, COLstring* pError);

#endif // end of defensive include
