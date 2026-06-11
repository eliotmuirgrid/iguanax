#ifndef __CACHE_FILE_H__
#define __CACHE_FILE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CACHEfile
//
// Description:
//
// Abstracted API to return files in the cache we ship with the binary
//
// Author: Eliot Muir 
// Date:   Wednesday 12 July 2023 - 04:26PM
// ---------------------------------------------------------------------------
class COLvar;
class COLstring;

bool CACHEload();
void CACHEsetCache(COLvar* pCache);
// NOTE that this will return GZIPPED content if pulled from cache and is not a help file
bool CACHEreadFile(const COLstring& Path, COLstring* pContent);

void CACHEfreeCache();

bool CACHEexists();

COLstring CACHEhash();
COLstring CACHEhelpHash();

void CACHEgetDeprecated(COLstring* pDeprecated);

// returns a COLvar list of {name, location}
// TODO could this could be more general - it's about getting a DIR list
// Should not need to expose COLvar
void CACHEgetHelpList(COLvar* pHelpVar);

void CACHEsaveCaBundle();

bool CACHEsetupDefaultVmd(COLstring* pVmdPath);

#endif // end of defensive include
