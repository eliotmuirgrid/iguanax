// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CHNKwriteStringVar
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Thursday 07 December 2023 - 12:47PM
// ---------------------------------------------------------------------------

#include <CHNK/CHNKwriteStringVar.h>
#include <CHNK/CHNKrenderChunks.h>
#include <CHNK/CHNKwriteChunk.h>

#include <COL/COLvar.h>
#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


int CHNKargLinesInLastChunk(const COLarray<CHNKstringArgChunk>& Chunks){
   COL_FUNCTION(CHNKargLinesInLastChunk);
   return Chunks.size() ? Chunks[Chunks.size()-1].m_Lines.size() : 0;
}

void CHNKwriteChunkVar(const COLarray<CHNKrenderedChunk>& RenderedChunks, const COLarray<CHNKstringArgChunk>& Chunks, COLvar* pResult){
   COL_FUNCTION(CHNKwriteChunkVar);
   COLvar& Result = *pResult;
   COLvar ChunksVar;
   ChunksVar.setArrayType();
   for (int i = 0; i < RenderedChunks.size(); i++){
      COLvar OneChunk;
      OneChunk["ChunkNumber"] = RenderedChunks[i].m_ChunkNumber;
      OneChunk["Value"] = RenderedChunks[i].m_Value;
      ChunksVar.push_back(OneChunk);
   }
   Result["Chunks"].swap(&ChunksVar);
   Result["TotalChunks"] = Chunks.size();
   Result["LinesInLastChunk"] = CHNKargLinesInLastChunk(Chunks);
}
