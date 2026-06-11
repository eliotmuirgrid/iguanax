#ifndef __HASH_UTILS_H__
#define __HASH_UTILS_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: HASHutils.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   05/04/24
//  ---------------------------------------------------------------------------
#include <COL/COLarray.h>

COLstring HASHparseError();
COLstring HASHerror();

bool HASHisValid(const COLstring& Tag);
bool HASHvarToArr(const COLvar& HashVar, COLarray<COLstring>* pArr);
COLstring HASHarrToString(const COLarray<COLstring>& HashArr);
void HASHstringToVar(const COLstring& HashString, COLvar* pHashVar);
#endif // end of defensive include
