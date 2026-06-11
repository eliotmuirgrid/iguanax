#ifndef __DB_DATABASE_MY_SQL_BASE_H__
#define __DB_DATABASE_MY_SQL_BASE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabaseMyBaseSql
//
// Description:
//
// This used to be the base class for DBdatabaseMySql* but since we no
// longer specialize the high-level DBdatabase interface for MySQL, it
// is no longer needed.  This header is for backwards compatibility.
//
// Author: Nasron Cheong
// Date:   June, 2007
//
//---------------------------------------------------------------------------

#include "DBdatabaseMySql.h"

// For backwards compatibility.
#define DBdatabaseMySqlBase  DBdatabaseMySql

#endif // end of defensive include
