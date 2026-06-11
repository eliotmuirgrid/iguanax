#ifndef __VERSION_H__
#define __VERSION_H__
// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: version.h
//
//  Description: runs the OS-specific binary to get the iguana version
//
//  Author: Matthew Sobkowski
//  Date:   Thursday August 17, 2023
// ---------------------------------------------------------------------------
class COLstring;

COLstring getVersion(const COLstring& Location);
bool createReleaseJson(const COLstring& Version);

#endif // end of defensive include
