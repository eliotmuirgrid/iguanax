//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: UTDBconnectInfo
//
// Description:
//
// Implementation
//
// Author: Dmitri Klimenko
// Date:   Friday, January 16th, 2009 @ 02:15:38 PM
//---------------------------------------------------------------------------
#include "UTDBprecomp.h"
#pragma hdrstop

#include "UTDBconnectInfo.h"

#include <DB/DBdatabase.h>
#include <DB/DBdatabasePtr.h>
// #include <DB/DBdatabase#Factory.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// JULIAN commenting out old tests to remove DBdatabase#Factory (delete file later)


// UTDBconnectInfo::UTDBconnectInfo(const COLstring& Api, const COLstring& DataSource, const COLstring& User, const COLstring& Password){
//    this->Api = Api;
//    this->DataSource = DataSource;
//    this->User = User;
//    this->Password = Password;
// }

// const COLstring& UTDBconnectInfo::api()        const { return Api; }
// const COLstring& UTDBconnectInfo::dataSource() const { return DataSource; }
// const COLstring& UTDBconnectInfo::user()       const { return User; }
// const COLstring& UTDBconnectInfo::password()   const { return Password; }

// COLostream& operator<<(COLostream& Stream, const UTDBconnectInfo& Obj){
//    Stream << "Api: " << Obj.api() << " Name: " << Obj.dataSource() << " User: " << Obj.user() << " Password: " << Obj.password();
//    return Stream;
// }

// COLstring UTDBchmTestDb() {
//    COLstring ChmTestDb = ::getenv("CHM_TEST_DB");
//    if(ChmTestDb.is_null()) {
//       ChmTestDb = "test";
//    }
//    return ChmTestDb;
// }

// COLstring UTDBhostMySql(){
//    COLstring Host = ::getenv("CHM_TEST_DB_HOST_MYSQL");
//    if(!Host.is_null())return Host;
//    return "mysql-server";
// }

// UTDBconnectInfo UTDBconnectMySql(){
//    COL_FUNCTION(UTDBconnectMySql);
//    COLstring ChmTestDb = UTDBchmTestDb();
//    COLstring DataSource = ChmTestDb + "@" + UTDBhostMySql();
//    COL_VAR(DataSource);
//    return UTDBconnectInfo(DBdatabase::DB_DATABASE_MY_SQL_NAME, DataSource, ChmTestDb, ChmTestDb);
// };

// UTDBconnectInfo UTDBconnectMySql64(){
//    COLstring ChmTestDb = UTDBchmTestDb();
//    COLstring DataSource = ChmTestDb + "@mysql64";
//    return UTDBconnectInfo(DBdatabase::DB_DATABASE_MY_SQL_NAME, DataSource, ChmTestDb, ChmTestDb);
// };

// UTDBconnectInfo UTDBconnectOracleOci(){
//    COLstring ChmTestDb = UTDBchmTestDb();
//    return UTDBconnectInfo( DBdatabase::DB_DATABASE_OCI_ORACLE_NAME,
//       "(description=(address=(host=oraclexe)(protocol=tcp)(port=1521))(connect_data=(sid=XE)))",
//       ChmTestDb, ChmTestDb);
// }

// UTDBconnectInfo UTDBconnectOracle64Oci(){
//    COLstring ChmTestDb = UTDBchmTestDb();
//    return UTDBconnectInfo( DBdatabase::DB_DATABASE_OCI_ORACLE_NAME,
//       "(description=(address=(host=oracle64)(protocol=tcp)(port=1521))(connect_data=(sid=ORCL)))",
//       "ut_" + ChmTestDb, "ut_" + ChmTestDb);
// }

// UTDBconnectInfo UTDBconnectOracleOdbc(){
//    COLstring ChmTestDb = UTDBchmTestDb();
//    return UTDBconnectInfo(DBdatabase::DB_DATABASE_ODBC_ORACLE_NAME, "OracleTest", ChmTestDb, ChmTestDb);
// }

// UTDBconnectInfo UTDBconnectPostgres(){
//    COLstring ChmTestDb = UTDBchmTestDb();
//    // one driver per machine, pre-configured with default database/user/password
//    return UTDBconnectInfo(DBdatabase::DB_DATABASE_ODBC_POSTGRESQL_NAME, "PostgreSQLTest",  "", "");
// }

// UTDBconnectInfo UTDBconnectMySqlOdbc(){
//    COLstring ChmTestDb = UTDBchmTestDb();
//    return UTDBconnectInfo(DBdatabase::DB_DATABASE_ODBC_MYSQL_NAME, "MyODBCTest", ChmTestDb, ChmTestDb);
// }

// UTDBconnectInfo UTDBconnectAccess(){
//    COLstring ChmTestDb = UTDBchmTestDb();
//    // one driver per machine, pre-configured with default database/user/password
//    return UTDBconnectInfo(DBdatabase::DB_DATABASE_ODBC_ACCESS_NAME, "AccessTest",  "",  "");
// }

// UTDBconnectInfo UTDBconnectSqlServer(){
//    COLstring ChmTestDb = UTDBchmTestDb();
//    return UTDBconnectInfo(DBdatabase::DB_DATABASE_SQL_SERVER_NAME, "SqlServerTest", ChmTestDb, ChmTestDb);
// }

// UTDBconnectInfo UTDBconnectSybaseAsa(){
//    COLstring ChmTestDb = UTDBchmTestDb();
//    return UTDBconnectInfo(DBdatabase::DB_DATABASE_ODBC_SYBASE_NAME, "SybaseAsaTest", ChmTestDb, ChmTestDb);
// }

// UTDBconnectInfo UTDBconnectSybaseAse(){
//    COLstring ChmTestDb = UTDBchmTestDb();
//    COLstring Password = ChmTestDb;
//    if(Password.size() < 6){
//       // for user "dev" - Sybase ASE requires minimum 6 chars in password
//       // - padding with underscores
//       for(COLindex CharIndex=Password.size(); CharIndex < 6; CharIndex++){
//          Password += "_";
//       };
//    };

//    return UTDBconnectInfo( DBdatabase::DB_DATABASE_ODBC_SYBASE_ASE_NAME, "SybaseAseTest", ChmTestDb, Password);
// }

// UTDBconnectInfo UTDBconnectInterbase(){
//    return UTDBconnectInfo(DBdatabase::DB_DATABASE_ODBC_INTERBASE_NAME, "InterbaseTest", "TEST_STATIC", "gh45iop");
// };

// UTDBconnectInfo UTDBconnectDb2(){
//    return UTDBconnectInfo(DBdatabase::DB_DATABASE_ODBC_DB2_NAME, "Db2Test", "",
//       "");
// };

// UTDBconnectInfo UTDBconnectInformix(){
//    return UTDBconnectInfo(DBdatabase::DB_DATABASE_ODBC_INFORMIX_NAME, "InformixTest", "test", "gh45iop");
// };

// UTDBconnectInfo UTDBconnectFilemaker(){
//    return UTDBconnectInfo(DBdatabase::DB_DATABASE_ODBC_FILEMAKER_NAME, "FilemakerTest", "admin", "gh45iop");
// };

// UTDBconnectInfo UTDBconnectSqlite(){
//    return UTDBconnectInfo( DBdatabase::DB_DATABASE_SQLITE_NAME, "SqliteTest.sqlite", "", "");
// }

// void UTDBreport(COLostream& Stream){
//    COL_FUNCTION(UTDBreport);
//    Stream << "From UTDB library, first environmental variables:"             << newline
//           << "CHM_TEST_DB="            << ::getenv("CHM_TEST_DB")            << newline
//           << "CHM_TEST_DB_HOST_MYSQL=" << ::getenv("CHM_TEST_DB_HOST_MYSQL") << newline
//           << UTDBconnectMySql()      << newline
//           << UTDBconnectMySqlOdbc()  << newline
//           << UTDBconnectOracleOci()  << newline
//           << UTDBconnectOracleOdbc() << newline 
//           << UTDBconnectPostgres()   << newline
//           << UTDBconnectSqlServer()  << newline
//           << UTDBconnectSqlite()     << newline;
// }

// DBdatabasePtr UTDBconnect(DBdatabase#Factory& Factory, const UTDBconnectInfo& ConnectInfo){
//    // JULIAN removing the DBdatabase#Factory will break 
//    // TODO fix this - appears to be calling tests...
//    DBdatabasePtr Db = Factory.getDatabase(ConnectInfo.api());
//    Db->connect(ConnectInfo.dataSource().c_str(), ConnectInfo.user().c_str(), ConnectInfo.password().c_str());
//    return Db;
// };

// bool UTDBcheckConnect(DBdatabase#Factory& Factory, const UTDBconnectInfo& ConnectInfo){
//    try{
//       UTDBconnect(Factory,ConnectInfo);
//       return true;
//    } catch(...) {
//       return false;
//    };
// };