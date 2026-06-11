#ifndef __CHECKHASH_H__
#define __CHECKHASH_H__
// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: checkHash.h
//
//  Description: Contains all logic related to verifying given hash
//
//  Author: Matthew Sobkowski
//  Date:   Thursday August 17, 2023
// ---------------------------------------------------------------------------
class COLstring;
class COLvar;

bool downloadHash(const char* Url, const COLstring& Hash, const COLstring& File, const COLstring& Location);
bool requestOs(const char* OSurl, const COLstring& Hash, COLvar* pFileList);

#endif // end of defensive include
