#ifndef __IDB_DATABASE_CONFIG_H__
#define __IDB_DATABASE_CONFIG_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IDBdatabaseConfig
//
// Description:
//
// A configuration object for the various databases.
//
// Author: Vismay Shah
// Date:   Mon 5 Apr 2021 1:42:05 EDT
//---------------------------------------------------------------------------
#include "IDBmySql.h"
#include "IDBoracle.h"
#include "IDBodbc.h"
#include "IDBsqlite.h"
#include "IDBmariaDb.h"


class IDBdatabaseConfig{
public:
   const IDBmySql&   mySql()    const { return MySql; }
   IDBmySql&         mySql()          { return MySql; }
   const IDBoracle&  oracle()   const { return Oracle; }
   IDBoracle&        oracle()         { return Oracle; }
   const IDBodbc&    odbc()     const { return Odbc; }
   IDBodbc&          odbc()           { return Odbc; }
   const IDBsqlite&  sqlite()   const { return Sqlite; }
   IDBsqlite&        sqlite()         { return Sqlite; }
   const IDBmariaDb& mariadb()  const { return MariaDb; }
   IDBmariaDb&       mariadb()        { return MariaDb; }

private: 
   IDBmySql          MySql;
   IDBoracle         Oracle;
   IDBodbc           Odbc;
   IDBsqlite         Sqlite;
   IDBmariaDb        MariaDb;
};

bool IDBreadConfigFile(IDBdatabaseConfig* pConfig);
void IDBsaveConfigFile(IDBdatabaseConfig* pConfig);

#endif // end of defensive include
