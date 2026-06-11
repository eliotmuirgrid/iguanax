#ifndef __IDB_SET_H__
#define __IDB_SET_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IDBset
//
// Description:
//
// Api to save database driver locations
//
// Author: Vismay Shah 
// Date:   Wednesday 28 December 2022 - 12:06PM
// ---------------------------------------------------------------------------
class COLwebRequest;
class IDBdatabaseConfig;
class SCKloop;

void IDBsetDriverPaths(const COLwebRequest& Request, SCKloop* pLoop, IDBdatabaseConfig* pDatabaseConfig);

#endif // end of defensive include
