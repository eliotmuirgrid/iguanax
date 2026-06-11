#ifndef __GIT_FETCH_H__
#define __GIT_FETCH_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITfetch
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Friday 17 March 2023 - 10:43AM
// ---------------------------------------------------------------------------

class COLstring;

bool GITfetch(const COLstring& Username, const COLstring& Dir, const COLstring& SourceDir, COLstring* pOut, COLstring* pError);

#endif // end of defensive include
