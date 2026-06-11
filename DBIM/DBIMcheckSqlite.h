#ifndef __DBIM_CHECKSQLITE_H__
#define __DBIM_CHECKSQLITE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBIMcheckSqlite
//
// Description:
//
// Checks if a file is a SQLite file and lists its tables if so
//
// Author: Tyler Brown
// Date:   Thursday 02 February 2023 - 03:20PM
// ---------------------------------------------------------------------------
class COLwebRequest;
class SCKloop;

void DBIMcheckSqlite(const COLwebRequest& Request, SCKloop* pLoop);

#endif //IFWARE_MAIN2_DBIM_DBIMCHECKSQLITE_H_
