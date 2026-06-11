#ifndef __BUMP_H__
#define __BUMP_H__
// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: bump.h
//
//  Description: functions relative to bump flag
//
//  Author: Matthew Sobkowski
//  Date:   Friday, September 29, 2023
// ---------------------------------------------------------------------------
class COLstring;

COLstring extractVersionData(const COLstring& Content);
int bumpVersion();

#endif //end of defensive include
