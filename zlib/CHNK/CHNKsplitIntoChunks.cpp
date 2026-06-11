// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CHNKsplitIntoChunks
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Thursday 07 December 2023 - 11:35AM
// ---------------------------------------------------------------------------

#include <CHNK/CHNKsplitIntoChunks.h>
#include <CHNK/CHNKwriteChunk.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

#define CHNK_CHUNK_SIZE      200
void CHNKsplitIntoChunks(const COLstring& ViewString, COLarray<CHNKstringArgChunk>* pChunks){
   COL_FUNCTION(CHNKsplitIntoChunks);
   const COLstring& Original = ViewString;
   COLarray<CHNKstringArgChunk>& Chunks = *pChunks;
   Chunks.clear();
   COLarray<CHNKstringArgLine>* pCurrentLines = NULL;
   int Size = Original.size();
   int CurrentStart = 0, CurrentLength = 0;
   const char* pOriginal = Original.c_str();
   do {
      if (!pCurrentLines || pCurrentLines->size() == CHNK_CHUNK_SIZE) {
         CHNKstringArgChunk& NewChunk = Chunks.push_back();
         NewChunk.m_ChunkNumber = Chunks.size()-1;
         pCurrentLines = &NewChunk.m_Lines;
      }
      const char* pCurrent = pOriginal + CurrentStart;
      CurrentLength = ::strcspn(pCurrent, "\r\n");
      CHNKstringArgLine& Line = pCurrentLines->push_back();
      Line.m_pStart = pCurrent;
      Line.m_Size = CurrentLength;
      CurrentStart += CurrentLength;
      if (pOriginal[CurrentStart++] == '\r' && pOriginal[CurrentStart] == '\n'){
         CurrentStart++;
      }
   } while (CurrentStart < Size);
}