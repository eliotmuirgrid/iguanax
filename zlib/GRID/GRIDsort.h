#ifndef _GRIDSORT_H
#define _GRIDSORT_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDsort.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
class COLstring;
class GRIDnode;
class COLvar;

void GRIDsortPeers(COLvar* pResponse, GRIDnode* pNode, const COLstring& SortBy, bool Ascending);

#endif  // end of defensive include
