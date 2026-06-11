#ifndef __IDB_GET_H__
#define __IDB_GET_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IDBget
//
// Description:
//
// Api to get database driver locations
//
// Author: Vismay Shah 
// Date:   Wednesday 28 December 2022 - 12:09PM
// ---------------------------------------------------------------------------
class COLwebRequest;
class IDBdatabaseConfig;

void IDBgetDriverPaths(const COLwebRequest& Request, IDBdatabaseConfig* pDatabaseConfig);

#endif // end of defensive include
