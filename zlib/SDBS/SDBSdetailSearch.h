#ifndef SDBSDETAILSEARCH_H
#define SDBSDETAILSEARCH_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SDBSdetailSearch.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   29/07/25 2:15 PM
//  ---------------------------------------------------------------------------
class COLstring;
class COLwebRequest;
class SCKloop;
class SDBSdetailSearchItem;

void SDBSdetailSearchPoll(const COLwebRequest& Request, COLhashmap<COLstring, SDBSdetailSearchItem>* pSearchMap);

void SDBSdetailSearchStart(const COLwebRequest& Request, COLhashmap<COLstring, SDBSdetailSearchItem>* pSearchMap,
                              SCKloop* pLoop);

#endif // end of defensive include
