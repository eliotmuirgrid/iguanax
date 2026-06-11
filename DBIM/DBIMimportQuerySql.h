#ifndef __DBIM_IMPORT_QUERY_SQL_H__
#define __DBIM_IMPORT_QUERY_SQL_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBIMimportQuerySql
//
// Description:
//
// Sql statements for retrieving the table schema for various database types
//
// Author: Vismay Shah 
// Date:   Thursday 16 February 2023 - 04:18PM
// ---------------------------------------------------------------------------
#include <COL/COLstring.h>

COLstring DBIMmysqlGetTableData(const COLstring& DatabaseName); // Also for MariaDb
COLstring DBIMsqlServerGetTableData(const COLstring& DatabaseName);
COLstring DBIMsqliteGetTableData();
COLstring DBIMoracleGetTableData();
COLstring DBIMpostgresGetTableData();

#endif // end of defensive include
