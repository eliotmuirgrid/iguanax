#ifndef __NOB_COPY_H__
#define __NOB_COPY_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NOBcopy
//
// Description:
//
// NOBcopy will copy any NODplace tree into an NOB tree.  The original
// tree and all its underlying data can be deleted, and the copied NOB
// tree will still behave exactly like the original, except it will not
// be writeable.
//
// Author: Kevin Senn
// Date:   Monday, September 19th, 2011 @ 02:23:46 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <COL/COLref.h>

#include <NOD/NODplace.h>

// Calling code should assume ownership of the return value.
// This tree might be an entry in another tree.  If so, pKeyName can be
// provided to specify the key name for the entire tree.
NODplace* NOBcopy(const NODplace& Original, const char* pKeyName = NULL);

#endif // end of defensive include
