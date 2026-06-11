#ifndef __CHNK_SPLIT_INTO_CHUNKS_H__
#define __CHNK_SPLIT_INTO_CHUNKS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CHNKsplitIntoChunks
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Thursday 07 December 2023 - 11:35AM
// ---------------------------------------------------------------------------

class COLstring;
class CHNKstringArgChunk;

#include <COL/COLarray.h>

void CHNKsplitIntoChunks(const COLstring& ViewString, COLarray<CHNKstringArgChunk>* pChunks);

#endif // end of defensive include
