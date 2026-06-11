#ifndef __DB_DATABASE_TYPE_H__
#define __DB_DATABASE_TYPE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabaseType
//
// Description:
//
// Enumeration with all the database types.  This is intended
// to allow RTTI without needing TREe.
//
// Author: Eliot Muir
// Date:   Mon 11/26/2007 
//
//---------------------------------------------------------------------------

// Uses historical constant values pre IguanaX for compatibility (precaution)
enum DBdatabaseType
{
   DB_MY_SQL          = 1001,
   DB_ODBC_POSTGRESQL = 1002,
   DB_ODBC_SYBASE     = 12,   // backward compatibility old DB code JULIAN TODO remove later
   DB_ODBC_SYBASE_ASE = 1004, 
   DB_ODBC_ACCESS     = 1005,
   DB_SQL_SERVER      = 1006,
   DB_ODBC_DB2        = 1007,
   DB_ODBC_INFORMIX   = 1008,
   DB_ODBC_INTERBASE  = 1009,
   DB_ODBC_FILEMAKER  = 1010,
   DB_OCI_ORACLE      = 1011,
   DB_ODBC_ORACLE     = 1012,
   DB_SQLITE          = 1013,
   DB_MARIA_DB        = 1014,
   DB_ODBC_MYSQL      = 1015
};

// JULIAN TODO old enum declaration to remove later
// enum DBdatabaseType
// {
//    DB_MY_SQL,
//    DB_SQLITE,
//    DB_SQL_SERVER,
//    DB_OCI_ORACLE,
//    DB_ODBC_ORACLE,
//    DB_ODBC_ACCESS,
//    DB_ODBC_POSTGRESQL,
//    DB_ODBC_MYSQL,
//    DB_ODBC_FILEMAKER,
//    DB_ODBC_INTERBASE,
//    DB_ODBC_INFORMIX,
//    DB_ODBC_DB2,
//    DB_ODBC_SYBASE,
//    DB_ODBC_SYBASE_ASE,  // This used to be DB_ODBC_SYBASE too. :-/
//    DB_MARIA_DB
// };

#endif // end of defensive include
