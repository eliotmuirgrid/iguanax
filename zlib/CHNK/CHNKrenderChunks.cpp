// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CHNKrenderChunks
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Thursday 07 December 2023 - 12:29PM
// ---------------------------------------------------------------------------

#include <CHNK/CHNKrenderChunks.h>
#include <CHNK/CHNKwriteChunk.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;


void CHNKrenderChunks(const COLarray<CHNKstringArgChunk>& Chunks, int FirstChunkNumber, int SecondChunkNumber, COLarray<CHNKrenderedChunk>& RenderedChunks){
   COL_FUNCTION(CHNKrenderChunks);
   // If chunks are out of range, render the last two.
   if (FirstChunkNumber >= Chunks.size() || SecondChunkNumber >= Chunks.size()){
      FirstChunkNumber = COL_MAX(0, Chunks.size() - 2);
      SecondChunkNumber = Chunks.size() - 1;
   }
   CHNKrenderedChunk& FirstChunk = RenderedChunks.push_back();
   FirstChunk.m_ChunkNumber = FirstChunkNumber;
   CHNKwriteChunk(Chunks[FirstChunkNumber], FirstChunk.m_Value);
   if (SecondChunkNumber > FirstChunkNumber){
      CHNKrenderedChunk& SecondChunk = RenderedChunks.push_back();
      SecondChunk.m_ChunkNumber = SecondChunkNumber;
      CHNKwriteChunk(Chunks[SecondChunkNumber], SecondChunk.m_Value);
   }
}

