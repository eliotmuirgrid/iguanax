#ifndef __EDIHL7_H__
#define __EDIHL7_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: EDIhl7.h
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
class EDIdata;

bool EDIhl7Preprocess(COLstring* pMessage, COLstring* pError);
void EDIpostProcess(EDIdata* pMessage);

bool EDIgetMsh9(const COLstring& Data, COLstring* pResult);
bool EDIcheckMshFirst(const EDIcollection& Collection);

// Will return false it fails with the error message in pMatched.
bool EDIparseMshWithRules(const EDIcollection& Collection, const COLstring& Data, COLstring* pMatched);

bool EDIparseMsh(const EDIcollection& Collection, COLstring* pData, COLstring* pMatch, COLstring* pErr);

#endif
