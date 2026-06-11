#ifndef __IDB_SETUP_H__
#define __IDB_SETUP_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IDBsetup
//
// Description:
//
// Moved from DBD by Eliot - routines to initialize database drivers.
//
// Author: Julian Muir
// Date:   Sunday 18 September 2022 - 01:12PM
// ---------------------------------------------------------------------------
class IDBdatabaseConfig;
class DBdatabaseFactory;
class SCKloop;

void IDBsetup(IDBdatabaseConfig* pDatabaseConfig, DBdatabaseFactory* pDatabaseFactory, SCKloop* pLoop);

#endif // end of defensive include
