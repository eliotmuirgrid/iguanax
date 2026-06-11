#ifndef DASHLISTVIEW_H
#define DASHLISTVIEW_H
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DASHlistView
//
// Description:
//
// Functions for filtering and sorting components for the list view.
//
// Author: Tyler Brown
// Date:   Oct 7 2021
//---------------------------------------------------------------------------
#include <COL/COLarray.h>

struct CFGconfig;
struct CFGmap;

COLarray<COLstring> DASHvarToArray(const COLvar& In);

void DASHglobMatch(const COLstring& GlobTerms, CFGmap* pMap, bool ExactMatch, COLarray<COLstring>* pComponents);
void DASHcombineLists(const COLarray<COLstring>& Selected, CFGmap* pMap, COLarray<COLstring>* pComponents);

#endif