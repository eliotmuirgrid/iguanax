#ifndef __GIT_PULL_H__
#define __GIT_PULL_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITpull
//
// Description:
//
// GIT pull operation
//
// Author: Eliot Muir 
// Date:   Saturday 01 April 2023 - 02:04PM
// ---------------------------------------------------------------------------

class COLstring;

bool GITpull(const COLstring& Username, const COLstring& Dir, COLstring* pOut, COLstring* pError);

#endif // end of defensive include
