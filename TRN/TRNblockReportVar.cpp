//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNblockReportVar
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday, November 22nd, 2010 @ 10:58:18 AM
//---------------------------------------------------------------------------
#include <TRN/TRNfile.h>
#include <TRN/TRNfileFind.h>
#include <TRN/TRNannotationState.h>
#include <TRN/TRNwriteFileInfo.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void TRNblockReportVar(const TRNannotationState& State, COLvar* pReportVar, const COLhashmap<COLstring, TRNfile*>& LuaFileList, const COLstring& FileName, int BlockLine) {
   COL_FUNCTION(TRNblockReportVar);
   const TRNfile* pFile = TRNfileFind(LuaFileList, FileName);
   COLPRECONDITION(pFile != NULL);
   const TRNfile& File = *pFile;

   const TRNfunction* pFunc = TRNfileFunctionAtLine(File,BlockLine);
   COLPRECONDITION(pFunc !=NULL);
   COL_VAR(pFunc->m_Annotations);
   TRNblockVar(State, pFunc, pReportVar);
}
