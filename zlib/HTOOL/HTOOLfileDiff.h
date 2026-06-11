// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: HTOOLfileDiff.h
//
//  Description
//
//  Author: Matthew Sobkowski
//  Date:   10/19/22 1:13 PM
// ---------------------------------------------------------------------------

#ifndef __GIT_FILE_DIFF_H__
#define __GIT_FILE_DIFF_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HTOOLfileDiff
//
// Description: API handler for the file diff view
//
// Author: Shuo Hou
// Date:   June 22 2022
//---------------------------------------------------------------------------
class COLwebRequest;

void HTOOLfileDiff(const COLwebRequest& Request);
bool HTOOLcheckDiffParameters(const COLstring& FileName, const COLstring& FilePath, const bool& FileExist, COLvar* pResult);
void HTOOLgetDiffContent(const COLstring& ProjectPath, const COLstring& CommitId, const COLstring& FileName, COLvar* pResult);

#endif