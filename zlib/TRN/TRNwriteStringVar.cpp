//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNwriteStringVar
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday, November 24th, 2010 @ 01:19:34 PM
//
//---------------------------------------------------------------------------
#include "TRNwriteStringVar.h"
#include <TRN/TRNargumentRequest.h>

#include <CHNK/CHNKwriteStringVar.h>
#include <CHNK/CHNKrenderChunks.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// FileName and FunctionName should be HTML escaped.
void TRNwriteStringVar(const COLarray<CHNKrenderedChunk>& Chunks, const TRNarg& Arg, COLvar* pResult) {
   COL_FUNCTION(TRNwriteStringVar);
   COLvar& Result = *pResult;
   CHNKwriteChunkVar(Chunks, Arg.m_Chunks, &Result["ChunkInfo"]);

   // We don't want a completely empty title.
   if (Arg.m_Title.is_null()){
      if (Chunks[0].m_Value.is_null()){
         Result["title"] = " ";
      } else {
         Result["title"] = Chunks[0].m_Value;
      }
   } else {
      Result["title"] = Arg.m_Title;
   }
   Result["file_name"] = Arg.m_FileName;
   Result["funct_name"] = Arg.m_FunctionName;
   Result["call"] = Arg.m_Call;
   Result["line"] = Arg.m_Line;
   Result["pt"] = Arg.m_ProtocolType;
   Result["nt"] = Arg.m_NodeType;
}
