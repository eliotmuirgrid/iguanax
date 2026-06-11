#ifndef __TRNAUTOCOMPLETEEND_H__
#define __TRNAUTOCOMPLETEEND_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: TRNautocompleteEnd.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   13/12/23 3:24 PM
//  ---------------------------------------------------------------------------
class COLstring;
class COLvar;
class TRNannotationState;

void TRNautocompleteEnd(const COLstring& Code, int Line, COLvar* pResult, TRNannotationState* pState);

#endif // end of defensive include
