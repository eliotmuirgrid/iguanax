#ifndef __TRN_DECODE_SAMPLE_DATA_FILE_H__
#define __TRN_DECODE_SAMPLE_DATA_FILE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNdecodeSampleDataFile
//
// Description:
//
// Helper functions to load and save translator files which might be sample data.
//
// Author: Eliot Muir
// Date:   Tuesday, November 16th, 2010 @ 02:58:17 PM
//---------------------------------------------------------------------------

class COLstring;

void TRNdecodeSampleDataFile(const COLstring& FullPath, const COLstring& ComponentId, COLstring* pCode);

bool TRNloadTranslatorFile(const COLstring& FileName, COLstring* pContent);

bool TRNisAllowedExtension(const COLstring& Extension);

#endif // end of defensive include
