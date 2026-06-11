//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNwriteChunk
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday, November 24th, 2010 @ 01:19:34 PM
//
//---------------------------------------------------------------------------
#include "TRNwriteChunk.h"
#include <SFI/SFIhtmlCrop.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// Find the starting point and length of each line in the string, and
// constructs chunks of lines.  Arg.m_ViewString should be set before calling.
//
#define TRN_MAX_LINE_LENGTH 64000

// TRNstringArgLine::TRNstringArgLine() : m_pStart(NULL), m_Size(0) {
//    COL_METHOD(TRNstringArgLine::TRNstringArgLine);
// }

// void TRNwriteChunk(const TRNstringArgChunk& Chunk, COLstring& Out){
//    COL_FUNCTION(TRNwriteChunk);
//    const COLarray<TRNstringArgLine>& Lines = Chunk.m_Lines;
//    for (int i = 0, CountOfLine = Lines.size(); i < CountOfLine; i++){
//       if (i) {
//          Out.write("\n", 1);
//       }
//       const TRNstringArgLine& Line = Lines[i];
//       if (Line.m_Size > TRN_MAX_LINE_LENGTH){
//          SFIhtmlCrop(Line.m_pStart, Line.m_Size,
//             "<span class=\"line_cropped_warning\">&hellip; This line is too long to display.</span>",
//             TRN_MAX_LINE_LENGTH, Out);
//       } else {
//          Out.write(Line.m_pStart, Line.m_Size);
//       }
//    }
// }
