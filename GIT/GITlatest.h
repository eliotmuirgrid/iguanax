#ifndef __GIT_LATEST_H__
#define __GIT_LATEST_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITlatest
//
// Description:
//
// Get the latest commit from a repo
//
// Author: Matthew Sobkowskiw 
// Date:   Friday 25 October 2024 - 10:00AM
// ---------------------------------------------------------------------------
class COLstring;

bool GITlatest(const COLstring& Dir, COLstring* pOut, COLstring* pError);

#endif // end of defensive include
