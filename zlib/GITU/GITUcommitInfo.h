#ifndef __GITU_COMMIT_INFO_H__
#define __GITU_COMMIT_INFO_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUcommitInfo
//
// Description:
//
// This function returns commit information for the given GIT repository.
//
// Author: Eliot Muir 
// Date:   Monday 27 March 2023 - 01:31PM
// ---------------------------------------------------------------------------
class COLvar;
class COLstring;

bool GITUcommitInfo(const COLstring& Dir, COLvar* pResult);

bool GITUcommitInfoInString(const COLstring& Dir, COLstring* pResult);

#endif // end of defensive include
