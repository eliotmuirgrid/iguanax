#ifndef __HELPENUMERATE_H__
#define __HELPENUMERATE_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: HELPenumerate.h
//
//  Description: Enumerates help files whether on disc or in cache.
//  Can take COLstring* if you want to get back the name of a function that was modified by user (ex. HELPlist)
//
//  Author: Matthew Sobkowski
//  Date:   30/11/23 12:21 PM
//  ---------------------------------------------------------------------------
#include <COL/COLhashmap.h>

void HELPenumerateHelp(COLhashmap<COLstring, COLstring>* pMap, const COLstring& Directory,
                       const COLstring& BaseFunction, COLstring* pName=nullptr);

#endif // end of defensive include
