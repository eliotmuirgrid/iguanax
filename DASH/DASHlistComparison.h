#ifndef __DASH_LIST_COMPARISON_H__
#define __DASH_LIST_COMPARISON_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DASHlistComparison
//
// Description:
//
// Functions to compare components for the list view.
//
// Author: Tyler Brown
// Date:   Oct 7 2021
//---------------------------------------------------------------------------
struct CFGconfig;

using DASHcompareFunc = bool (*)(CFGconfig* pComponent1, CFGconfig* pComponent2, bool Ascending, COLstring Column);
DASHcompareFunc DASHgetCompareFunction(const COLstring& Column);

bool DASHmpsiCompare(CFGconfig* pComponent1, CFGconfig* pComponent2, bool Ascending, COLstring Column);
bool DASHmpsoCompare(CFGconfig* pComponent1, CFGconfig* pComponent2, bool Ascending, COLstring Column);
bool DASHnameCompare  (CFGconfig* pComponent1, CFGconfig* pComponent2, bool Ascending, COLstring Column);
bool DASHlightCompare (CFGconfig* pComponent1, CFGconfig* pComponent2, bool Ascending, COLstring Column);
bool DASHerrorCompare (CFGconfig* pComponent1, CFGconfig* pComponent2, bool Ascending, COLstring Column);
bool DASHqueueCompare (CFGconfig* pComponent1, CFGconfig* pComponent2, bool Ascending, COLstring Column);
bool DASHserverCompare(CFGconfig* pComponent1, CFGconfig* pComponent2, bool Ascending, COLstring Column);
bool DASHcustomCompare(CFGconfig* pComponent1, CFGconfig* pComponent2, bool Ascending, COLstring Column);

int DASHtoUpperMod(int Char);
int DASHicompare(const char* s1, const char* s2);

#endif // end of defensive include