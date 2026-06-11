// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CHNKwriteChunk
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Thursday 07 December 2023 - 11:41AM
// ---------------------------------------------------------------------------

#include <CHNK/CHNKwriteChunk.h>

#include <SFI/SFIhtmlCrop.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// Find the starting point and length of each line in the string, and
// constructs chunks of lines.  Arg.m_ViewString should be set before calling.
//
#define CHNK_MAX_LINE_LENGTH 64000

CHNKstringArgLine::CHNKstringArgLine() : m_pStart(NULL), m_Size(0) {
   COL_METHOD(CHNKstringArgLine::CHNKstringArgLine);
}

void CHNKwriteChunk(const CHNKstringArgChunk& Chunk, COLstring& Out){
   COL_FUNCTION(CHNKwriteChunk);
   const COLarray<CHNKstringArgLine>& Lines = Chunk.m_Lines;
   for (int i = 0, CountOfLine = Lines.size(); i < CountOfLine; i++){
      if (i) {
         Out.write("\n", 1);
      }
      const CHNKstringArgLine& Line = Lines[i];
      if (Line.m_Size > CHNK_MAX_LINE_LENGTH){
         SFIhtmlCrop(Line.m_pStart, Line.m_Size,
            "<span class=\"line_cropped_warning\">&hellip; This line is too long to display.</span>",
            CHNK_MAX_LINE_LENGTH, Out);
      } else {
         Out.write(Line.m_pStart, Line.m_Size);
      }
   }
}