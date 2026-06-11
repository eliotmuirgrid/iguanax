#ifndef __CHNK_WRITE_STRING_VAR_H__
#define __CHNK_WRITE_STRING_VAR_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CHNKwriteStringVar
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Thursday 07 December 2023 - 12:47PM
// ---------------------------------------------------------------------------

#include <COL/COLarray.h>

class CHNKrenderedChunk;
class CHNKstringArgChunk;

void CHNKwriteChunkVar(const COLarray<CHNKrenderedChunk>& RenderedChunks, const COLarray<CHNKstringArgChunk>& Chunks, COLvar* pResult);

#endif // end of defensive include

