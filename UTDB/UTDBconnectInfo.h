#ifndef __UTDB_CONNECT_INFO_H__
#define __UTDB_CONNECT_INFO_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: UTDBconnectInfo
//
// Description:
//
// UTDBconnectInfo class.
//
// Author: Dmitri Klimenko
// Date:   Friday, January 16th, 2009 @ 02:15:38 PM
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

// class DBdatabase#Factory;
class DBdatabasePtr;

// JULIAN commenting out old tests to remove DBdatabase#Factory (delete file later)


// class UTDBconnectInfo {
// public:
//    UTDBconnectInfo(const COLstring& Api, const COLstring& DataSource, const COLstring& User, const COLstring& Password);

//    const COLstring& api() const;
//    const COLstring& dataSource() const;
//    const COLstring& user() const;
//    const COLstring& password() const;

// private:
//    COLstring Api;
//    COLstring DataSource;
//    COLstring User;
//    COLstring Password;

//    // copying this is OK - it's POD
//    // UTDBconnectInfo(const UTDBconnectInfo& Orig); // not allowed
//    // UTDBconnectInfo& operator=(const UTDBconnectInfo& Orig); // not allowed
// };

// COLostream& operator<<(COLostream& Stream, const UTDBconnectInfo& Obj);

// COLstring UTDBchmTestDb(); // can be used directly if it's simple setup

// UTDBconnectInfo UTDBconnectMySql();
// UTDBconnectInfo UTDBconnectMySql64();
// UTDBconnectInfo UTDBconnectMySqlOdbc();
// UTDBconnectInfo UTDBconnectOracleOci();
// UTDBconnectInfo UTDBconnectOracle64Oci();
// UTDBconnectInfo UTDBconnectOracleOdbc();
// UTDBconnectInfo UTDBconnectPostgres();
// UTDBconnectInfo UTDBconnectSqlServer();
// UTDBconnectInfo UTDBconnectSybaseAsa();
// UTDBconnectInfo UTDBconnectSybaseAse();
// UTDBconnectInfo UTDBconnectInterbase();
// UTDBconnectInfo UTDBconnectAccess();
// UTDBconnectInfo UTDBconnectDb2();
// UTDBconnectInfo UTDBconnectInformix();
// UTDBconnectInfo UTDBconnectFilemaker();
// UTDBconnectInfo UTDBconnectSqlite();

// COLstring UTDBhostMySql();

// // Print report of database connection information.
// void UTDBreport(COLostream& Stream);

// DBdatabasePtr UTDBconnect(DBdatabase#Factory& Factory, const UTDBconnectInfo& ConnectInfo);

// bool UTDBcheckConnect(DBdatabase#Factory& Factory, const UTDBconnectInfo& ConnectInfo);

#endif // end of defensive include
