#ifndef __GIT_RESET_H__
#define __GIT_RESET_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITreset
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Thursday 11 May 2023 - 12:31PM
// ---------------------------------------------------------------------------

class COLstring;

bool GITreset(const COLstring& Dir, const COLstring& Branch, COLstring* pOut, COLstring* pError);

#endif // end of defensive include
