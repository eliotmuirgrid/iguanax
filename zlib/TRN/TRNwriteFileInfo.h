#ifndef __TRN_WRITE_FILE_INFO_H__
#define __TRN_WRITE_FILE_INFO_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNwriteFileInfo
//
// Description:
//
// Helper to write block annotation data for the file in question
//
// Author: Eliot Muir
// Date:   Monday, November 22nd, 2010 @ 10:58:18 AM
//---------------------------------------------------------------------------

class COLvar;
class TRNfile;
class TRNannotationState;

void TRNwriteFileInfo(const TRNannotationState& State, TRNfile* pFile, COLvar* pResult);

void TRNblockVar(const TRNannotationState& State, const TRNfunction* pFunc, COLvar* pResult);

#endif // end of defensive include
