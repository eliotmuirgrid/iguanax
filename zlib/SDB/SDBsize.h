#ifndef __SDB_SIZE_H__
#define __SDB_SIZE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBsize
//
// Description:
//
// Given a directory this routine recursively finds the size.
//
// Author: Eliot Muir 
// Date:   Thursday 11 May 2023 - 01:26PM
// ---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class COLstring;

COLint64 SDBsize(const COLstring& Dir);

#endif // end of defensive include
