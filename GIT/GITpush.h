#ifndef __GIT_PUSH_H__
#define __GIT_PUSH_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITpush
//
// Description:
//
// GIT push operation
//
// Author: Tyler Brown 
// Date:   Tuesday 25 April 2023 - 12:07PM
// ---------------------------------------------------------------------------

class COLstring;

bool GITpush(const COLstring& Username, const COLstring& Dir, const COLstring& Flags, COLstring* pOut, COLstring* pError);

#endif // end of defensive include
