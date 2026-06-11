#ifndef __DBIM_READ_H__
#define __DBIM_READ_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBIMread
//
// Description:
//
// Querys the table schema from a database and returns a vdb formatted JSON string.
// Used in the vdb importer.
//
// Author: Tyler Brown 
// Date:   Thursday 02 February 2023 - 03:20PM
// ---------------------------------------------------------------------------

class COLwebRequest;
class DBdatabaseFactory;
class SCKloop;

void DBIMread(const COLwebRequest& Request, SCKloop* pLoop, DBdatabaseFactory* pDatabaseFactory);

#endif // end of defensive include
