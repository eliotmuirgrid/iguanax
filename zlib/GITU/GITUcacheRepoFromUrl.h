#ifndef GITUCACHEREPOSITORYFROMURL_H
#define GITUCACHEREPOSITORYFROMURL_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GITUcacheRepoFromUrl.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   03/10/24 4:06 PM
//  ---------------------------------------------------------------------------
class COLstring;

bool GITUcacheRepoFromUrl(const COLstring& Username, const COLstring& GitUrl, const COLstring& Branch, COLstring* pOutput);

#endif // end of defensive include
