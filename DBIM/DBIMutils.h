#ifndef __DBIM_UTILS_H__
#define __DBIM_UTILS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBIMutils
//
// Description:
//
// Utility functions used throughout the DBIM library.
//
// Author: Vismay Shah 
// Date:   Tuesday 07 November 2023 - 04:15PM
// ---------------------------------------------------------------------------
class COLstring;
class DBcollection;

bool DBIMresolveVdbFile(const COLstring& Vdb, const COLstring& Component, COLstring* pVdbFullPath);

bool DBIMreadVdbToDbCollection(const COLstring& Filepath, const COLstring& Component, COLstring* ResolvedVdb, DBcollection* pOut, COLstring* pError);

#endif // end of defensive include
