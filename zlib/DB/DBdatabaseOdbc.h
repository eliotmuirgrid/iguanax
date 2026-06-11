#ifndef __DB_DATABASE_ODBC_H__
#define __DB_DATABASE_ODBC_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabaseOdbc
//
// Description:
//
// This class handles calls to the ODBC C API. This class links the
// API with the common interface provided by the DBdatabase parent class.
//
// Author: Henry Tran
// Date:   February 23, 2004 
//
//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// JULIAN-CHECK went through file and commented out things that are not used
// and commented on other things also
//
// NOTE: I put 4 slashes "////" in front of each commented out function so 
//       it is easy to see them - and still easy to uncomment press ctrl+/ 2x
//
// Search for "JULIAN-CHECK" to find comments
/////////////////////////////////////////////////////////////////////////////
// NOTE: Most of the other DBdatabase[dbname] files will be very similar
//       but I may not actually go through them all at present... 
/////////////////////////////////////////////////////////////////////////////

#include "DBdatabase.h"
#include <COL/COLarray.h>

#include <IDB/IDBdatabaseConfig.h>


// JULIAN-CHECK class DBdatabaseOdbcPrivate will be replaced by DBdriverOdbc
// - 4 DB/DBdatabaseOdbc.h
// - 28 DB/DBdatabaseOdbc.cpp
class DBdatabaseOdbcPrivate;
// JULIAN-CHECK class DBodbcStatement - not sure how this used 
// - 5 DB/DBdatabaseOdbc.h
// - 33 DB/DBdatabaseOdbc.cpp
class DBodbcStatement;

// Instead of including the following file we are making typedefs for the ODBC
// typedefs ourselves.  The compiler should warn us if and when things change since
// in the cpp file we are using the types defined in sqltypes.h
//#include <sqltypes.h>
typedef void* DB2odbcSQLHANDLE;
typedef short DB2odbcSQLSMALLINT;

// JULIAN-CHECK class DBodbcStatement - not sure how this used 
// used in many ODBC files (but not all???)
struct DBodbcErrorList{ const char* Code; };

class DBdatabaseOdbc : public DBdatabase
{
public:
   DBdatabaseOdbc();
   virtual ~DBdatabaseOdbc();
      
   // JULIAN-CHECK this is no longer used in new implementation
   // commenting out = compile errors
   // Instances: 
   // - there appear to be many = code calling them can be removed/changed
   // - used a lot in older unit tests that will need to be replaced
   static bool initialize(COLostream& LogStream, const char* LibraryPath);
   static bool isInitialized();
   static const COLstring& loadedApiLibraryPath();

   // Initializes any required parameters/settings and connects to the database
   // JULIAN connect() function appears to be used as it is inherited from DBdatabase
   virtual void connect(const char* DataSourceName, const char* UserName, const char* Password);

   // Deallocates all open database handles and disconnects the 
   // DBdatabase object from the DBMS.
   // JULIAN disconnect() function appears to be used as it is inherited from DBdatabase
   virtual void disconnect();

   // The following function is defined in the parent class DBdatabase:
   // Discovers the type of SQL command at run time and calls one of the
   // specific SQL command execution methods below.
   //
   // virtual DBresultSetPtr executeSqlCommand( const DBsqlCommand& SqlCommand ) const;

   // Returns if the database object is connected. This can be used to test
   // the connection before issuing further statements to the object.
   // Note that databases may go down, which may result in databases disconnecting on
   // exceptions, and not on just calls to disconnect().
   // JULIAN hasConnectionByGuid() function appears to be used as it is inherited from DBdatabase
   virtual bool isConnected();

   // JULIAN folowing 2 functions appear to be used as they are inherited from DBdatabase
   virtual DBresultSetPtr executeSqlInsert( const DBsqlInsert& SqlInsertCommand ) const;
   virtual DBresultSetPtr executeSqlUpdate( const DBsqlUpdate& SqlUpdateCommand ) const;

   // Submits a SQL command string to the DBMS. All of the above executeSql methods call this method.
   // NOTE: Input strings are assumed to be unescaped as this function will escape any special SQL characters for you.
   // JULIAN executeSqlString() function appears to be used as it is inherited from DBdatabase
   virtual DBresultSetPtr executeSqlString( const COLstring& SqlString,
                                            COLuint32 ResultSetStart = 0,
                                            COLuint32 ResultSetSize = 0,
                                            COLarray<DBresultSetPtr> *pAllResultSets = NULL) const;

   // Since ODBC uses parameter binding, this function needs to be overriden.
   // JULIAN-CHECK this appears to be legit overide (of createSqlCommandWithoutBinding from DBdatabase)
   virtual void createSqlCommandWithoutBinding( const DBsqlCommand& SqlCommand, COLostream& OutputSql ) const;

   // Stores a list of database table names into a DBresultSet object.
   // Access this list through the resultSet() function above.
   // JULIAN fetchDatabaseTables() function appears to be used as it is inherited from DBdatabase
   virtual DBresultSetPtr fetchDatabaseTables() const;

   // Stores a list of non-system database table names into a DBresultSet object.
   // JULIAN-CHECK fetchNonSystemDatabaseTables functions never used - commented out here and DB/DBdatabaseOdbc.cpp
   // commented out = no compile errors
   //// virtual DBresultSetPtr fetchNonSystemDatabaseTables() const;

   // Stores column information of a specified database table into a DBresultSet
   // object. Parameters such as column name, data type (in terms of 
   // DBdataType), default value (if any), length, required column
   // flag, and primary key flag are stored as separate result set columns 
   // (the result set column names denote which result set column corresponds 
   // to which parameter).
   // 
   // Access this information through the resultSet() function above.
   // JULIAN-CHECK fetchNonSystemDatabaseTables functions
   // this is used in other ODBC files that inherit from this one
   // BUT IT MAY WELL BE THAT IN THE OTHER FILES IT IS NOT ACTUALLY USED 
   virtual DBresultSetPtr fetchDatabaseTableColumns( const char* TableName ) const;

   // Returns true if the table with the specified table name is a system
   // table, false otherwise.
   // JULIAN isSystemTable() used in Oracle ODBC by inheritance
   virtual bool isSystemTable( const char* TableName ) const;

   // JULIAN-CHECK fetchNonSystemDatabaseTables functions never used 
   // commented out = no compile errors 
   // BUT THIS IS PROBABLY NEEDED - so I did not comment out
   virtual void setAutocommitFlag( bool Flag ) const;
   // ================================================================================

   // JULIAN-CHECK 2 functions never used 
   // commented out = no compile errors 
   // BUT THESE ARE NEEDED - so I did not comment out
   virtual void commitTransaction();
   virtual void rollbackTransaction();

   // JULIAN-CHECK we will replace this with DBdriverOracleOci
   // but it causes compile errors so not commented out...
   friend class DBdatabaseOdbcPrivate;
   // JULIAN-CHECK this is used in DBdatabaseOdbc.cpp
   // but we may want to change the implementation as it seems "ODD" I think...
   virtual bool supportsLongVarChar() const {return true;}

   //this is the string used to determine how to check if the connection is alive
   //overridden in some implementations
   // JULIAN-CHECK this is used in DBdatabaseOdbc.cpp - seems useful
   virtual const COLstring& sqlPingCommand() const;

   // sets the timeout for operations
   // JULIAN-CHECK this is used in DBdatabaseOdbc.cpp - seems useful
   virtual void setTimeout(COLuint32 Seconds);
   // JULIAN-CHECK this is actually NOT used in DBdatabaseOdbc.cpp - but seems useful
   virtual COLuint32 timeout() const;

   void setParameter(const COLstring& Name, const COLstring& Value); // Important! Need this to support things like UseUnicode

   // JULIAN-CHECK useBinding functions seems to be needed 
   // commented out = compile errors 
    bool useBinding() const;

   // whether we want to use WideChar API (UTF-16) (true),
   // or we're better off without complicating things any further (false),
   // to be overridden by databases which have a charset option in the driver
   // and/or a "set names utf8", which allows them to work with UTF8 directly
   // JULIAN-CHECK this is used in DBdatabaseOdbc.cpp
   virtual bool useWideChar() const;

   // JULIAN-CHECK this is used in DBdatabaseOdbc.cpp
   const COLstring& dbStringType() const;

   // JULIAN-CHECK this is used in DBdatabaseOdbc.cpp
   void streamDateTime(COLostream& Stream, const COLdateTime& DateTime ) const;

protected:
   bool useUnicode() const;

   virtual bool supportsDescribeParam() const;
   // special for Sybase ASE: do "" from " "
   virtual void adjustEmptyString(COLstring& ColumnData) const {}

   virtual void streamInsertUpdateValue( COLostream& Stream, 
                                         const DBsqlInsert& SqlCommand, 
                                         COLuint32 ColumnIndex ) const;
   virtual bool supportBinaryType() const;
   virtual void setSupportBinaryType (bool Tran);

   virtual void buildPrimaryKeyList( COLarray<COLstring>& PrimaryKeyList, const char* TableName ) const;
   
private:

   //Helper functions for hasConnectionByGuid.
   virtual COLuint32 acceptableErrorStringSize() const;
   virtual COLstring acceptableErrorString(COLuint32 ErrorIndex) const;
   //test if the the connection exists.
   virtual bool connectionExists();
   //test that a new statement can be constructed without throwing an exception
   virtual DBodbcStatement* statementSucceeds();
   //test that the server can be pinged
   virtual bool pingSucceeds(DBodbcStatement* pStatement);
   //if the ping failed, test that the errors is not something acceptable (ex. syntax error on interbase)
   virtual bool handledErrorIsOk(DBodbcStatement* pStatement);

   DBresultSetPtr executeSqlInsertOrUpdate(const DBsqlInsert&,
      void (*)(const DBdatabase*, const DBsqlInsert&, COLostream&)) const;

   DBdatabaseOdbcPrivate* pMember;

   DBdatabaseOdbc( DBdatabaseOdbc& Orig ); // not allowed
   DBdatabaseOdbc& operator=( DBdatabaseOdbc& Orig ); // not allowed
};

#endif // end of defensive include
