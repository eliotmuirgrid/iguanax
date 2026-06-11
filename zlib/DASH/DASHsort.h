#ifndef DASHSORT_H
#define DASHSORT_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: DASHsort.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   10/07/25 1:57 PM
//  ---------------------------------------------------------------------------
#include <COL/COLarray.h>

struct CFGconfig;
struct CFGmap;

typedef bool (*DASHcompareFunc)(CFGconfig* pComponent1, CFGconfig* pComponent2, bool Ascending, COLstring Column);

void DASHsort(const COLstring& Order, bool Ascending, CFGmap* pMap, COLarray<COLstring>* pResult);

#endif // end of defensive include
