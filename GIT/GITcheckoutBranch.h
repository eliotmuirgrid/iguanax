#ifndef _GITCHECKOUTBRANCH_H
#define _GITCHECKOUTBRANCH_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GITcheckoutBranch.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
class COLstring;

bool GITcheckoutBranch(const COLstring& Dir, const COLstring& Branch, const COLstring& Username, COLstring* pError);


#endif  // end of defensive include
