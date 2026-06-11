#ifndef __EXPT_DIR_ENUMCD_H__
#define __EXPT_DIR_ENUMCD_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: EXPTdirEnumcd
//
// Description:
//
// API to enumerate over a folder and read all files into a single string
// Files are delimited by \n%FILE <path>\n
//
// Author: Vismay Shah 
// Date:   Monday 17 November 2025 - 10:23AM
// ---------------------------------------------------------------------------
class COLstring;
class COLvar;

void EXPTgetAllFilesStrRecursive(const COLstring& Root, const COLstring& CurrentDir, bool IgnoreSamples, COLstring* pOutput, COLvar* pSkipped);
void EXPTcaptureSpecificFile(const COLstring& Root, const COLstring& File, COLstring* pOutput, COLvar* pSkipped);

#endif // end of defensive include
