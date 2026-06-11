#ifndef __CHNK_RENDER_CHUNKS_H__
#define __CHNK_RENDER_CHUNKS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CHNKrenderChunks
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Thursday 07 December 2023 - 12:29PM
// ---------------------------------------------------------------------------
#include <COL/COLarray.h>

class CHNKstringArgChunk;

// default copy constructor allowed.
class CHNKrenderedChunk{
public:
   int m_ChunkNumber;
   COLstring m_Value;
};

void CHNKrenderChunks(const COLarray<CHNKstringArgChunk>& Chunks, int FirstChunkNumber, int SecondChunkNumber, COLarray<CHNKrenderedChunk>& RenderedChunks);


#endif // end of defensive include
