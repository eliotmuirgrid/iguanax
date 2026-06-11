#ifndef __CHNK_WRITE_CHUNK_H__
#define __CHNK_WRITE_CHUNK_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CHNKwriteChunk
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Thursday 07 December 2023 - 11:41AM
// ---------------------------------------------------------------------------

#include <COL/COLarray.h>
// default copy constructor allowed.
class CHNKstringArgLine{
public:
   CHNKstringArgLine();
   const char* m_pStart;
   int m_Size;
};

// default copy constructor allowed.
class CHNKstringArgChunk{
public:
   CHNKstringArgChunk() : m_ChunkNumber(-1) {}

   int m_ChunkNumber;
   COLarray<CHNKstringArgLine> m_Lines;
};

void CHNKwriteChunk(const CHNKstringArgChunk& Chunk, COLstring& Out);

#endif // end of defensive include
