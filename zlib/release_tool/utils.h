#ifndef __UTILS_H__
#define __UTILS_H__
// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: utils.h
//
//  Description: Contains utility functions for DBDbuild
//
//  Author: Matthew Sobkowski
//  Date:   Thursday August 17, 2023
// ---------------------------------------------------------------------------
class COLstring;

bool reportMessage(const COLstring& Message, bool Success=true);
void cleanupRelease(const COLstring& Location);
void generateJson(const COLstring& Hash, const COLstring& Version, const COLstring& Location);
void generateReleaseJson(const COLstring& Version);
COLstring getTimestamp(const COLstring& Hash);

#endif // end of defensive include
