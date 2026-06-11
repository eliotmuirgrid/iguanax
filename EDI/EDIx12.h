#ifndef __EDIX12_H__
#define __EDIX12_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: EDIx12.h
//
//  Description:
//
//  Implementation
//
//  Author: Matthew Sobkowski
//  Date:   3/10/23 2:22 PM
//  ---------------------------------------------------------------------------
class COLstring;
class EDIcollection;

bool EDIx12Preprocess(COLstring* pMessage, COLstring* pError);

// Will return false it fails with the error message in pMatched.
bool EDIx12ParseWithRules(const EDIcollection& Collection, const COLstring& Data, COLstring* pMatched, COLstring* pErr);

bool EDIx12Parse(const EDIcollection& Collection, COLstring* pData, COLstring* pMessage, COLstring* pErr);

#endif // end of defensive include