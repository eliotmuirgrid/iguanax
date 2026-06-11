#ifndef SRCHCACHE_H
#define SRCHCACHE_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SRCHcache.h
//
//  Description: Multi-threaded routine to build a file cache map. Keys are full paths.
//
//  Author: Matthew Sobkowski
//  Date:   30/01/25 12:26 PM
//  ---------------------------------------------------------------------------
#include <COL/COLmap.h>

void SRCHreadIntoCache(COLmap<COLstring, COLstring>* pCache, const COLstring& Path);
void SRCHcache(COLmap<COLstring, COLstring>* pCache, const COLstring& Path, COLvar* pResult);

#endif  // end of defensive include
