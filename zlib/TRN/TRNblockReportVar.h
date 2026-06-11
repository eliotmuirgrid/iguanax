#ifndef __TRN_BLOCK_REPORT_VAR_H__
#define __TRN_BLOCK_REPORT_VAR_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNblockReportVar
//
// Description:
//
// Helper
//
// Author: Eliot Muir
// Date:   Monday, November 22nd, 2010 @ 10:58:18 AM
//
//---------------------------------------------------------------------------

#include <COL/COLhashmap.h>

class TRNfile;
class COLstring;
class COLvar;

void TRNblockReportVar(const TRNannotationState& State, COLvar* pReportVar, const COLhashmap<COLstring, TRNfile*>& LuaFileList, const COLstring& FileName, int BlockLine);

#endif // end of defensive include
