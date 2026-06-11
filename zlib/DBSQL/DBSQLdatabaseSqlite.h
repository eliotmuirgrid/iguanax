#ifndef __DBSQL_DATABASE_SQLITE_H__
#define __DBSQL_DATABASE_SQLITE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBSQLdatabaseSqlite
//
// Description:
//
// This class handles calls to a local SQLlite file.
//
// Author: Nasron Cheong
// Date:   July 2010 
//---------------------------------------------------------------------------

#include <DB/DBminimumInclude.h>
#include <DB/DBdatabase.h>
#include <SQL/SQLsqlite.h>

class DBresultSetRow;
template<class T> class COLarray;

class DBSQLdatabaseSqlite : public DBdatabase{
public:
   DBSQLdatabaseSqlite();
   ~DBSQLdatabaseSqlite();

   //since sqlite is an embedded db, pDataSourceName is the file name
   //user name and password are ignored
   void connect(const char* pDataSourceName, const char* pUserName, const char* pPassword); 
   void connect(const char* pDataSourceName, const char* pUserName, const char* pPassword, const char* pEncryptionKey);

   DBdatabaseType databaseType() const { return DB_SQLITE; }
   
   // Deallocates all open database handles and disconnects the 
   // DBdatabase object from the DBMS.
   void disconnect();

   const COLstring& dbStringType() const;
   const COLstring& dbIntegerType() const;
   const COLstring& dbDoubleType() const;
   const COLstring& dbDateTimeType() const; //mapped to double
   const COLstring& dbTextType() const;
   const COLstring& dbBinaryType() const;

   // The following function is defined in the parent class DBdatabase:
   // Discovers the type of SQL command at run time and calls one of the
   // specific SQL command execution methods below.
   //
   // virtual DBresultSetPtr executeSqlCommand( const DBsqlCommand& SqlCommand ) const;

   // Returns if the database object is connected. This can be used to test
   // the connection before issuing further statements to the object.
   // Note that databases may go down, which may result in databases disconnecting on
   // exceptions, and not on just calls to disconnect().
   bool isConnected();

   // Submits a SQL command string to the DBMS. All of the above executeSql...
   // methods call this method.
   //
   // NOTE: Input strings are assumed to be unescaped as this function will
   // escape any special SQL characters for you.
   DBresultSetPtr executeSqlString( const COLstring& SqlString,
                                    COLuint32 ResultSetStart = 0,
                                    COLuint32 ResultSetSize = 0,
                                    COLarray<DBresultSetPtr> *pAllResultSets = NULL) const;

   // Since sqlite does binding really should override this
   // JULIAN-CHECK this appears to be legit overide (of createSqlCommandWithoutBinding from DBdatabase)
  virtual void createSqlCommandWithoutBinding( const DBsqlCommand& SqlCommand, COLostream& OutputSql ) const;

   // Stores a list of database table names into a DBresultSet object.
   // Access this list through the resultSet() function above.
   DBresultSetPtr fetchDatabaseTables() const;

   // Stores a list of non-system database table names into a DBresultSet object.
   DBresultSetPtr fetchNonSystemDatabaseTables() const;

   // Stores column information of a specified database table into a DBresultSet
   // object. Parameters such as column name, data type (in terms of 
   // DBdataType), default value (if any), length, required column
   // flag, and primary key flag are stored as separate result set columns 
   // (the result set column names denote which result set column corresponds 
   // to which parameter).
   // 
   // Access this information through the resultSet() function above.
   DBresultSetPtr fetchDatabaseTableColumns( const char* TableName ) const;

   //Since sqlite is an embedded DB, this just returns an empty result set
   DBresultSetPtr fetchDatabases();

   // Returns true if the table with the specified table name is a system
   // table, false otherwise.
   bool isSystemTable( const char* TableName ) const;

   void setAutocommitFlag( bool Flag ) const;
   void commitTransaction();
   void rollbackTransaction();

   virtual COLostream& addEscapedStringToStream(COLostream& Stream, const COLstring& UnescapedString) const;

   bool useUnicode() const;

   void setParameter(const COLstring& Name, const COLstring& Value);

protected:
   virtual DBresultSetPtr executeSqlInsert( const DBsqlInsert& SqlInsertCommand ) const;
   virtual DBresultSetPtr executeSqlUpdate( const DBsqlUpdate& SqlUpdateCommand ) const;
   virtual void streamInsertUpdateValue( COLostream& Stream, 
      const DBsqlInsert& SqlCommand, 
      COLuint32 ColumnIndex ) const;
   virtual void streamBinary( COLostream& Stream, const COLstring &Buffer ) const;
   virtual void streamDateTime( COLostream& Stream, const COLdateTime& DateTime ) const;
   virtual bool selectUnionGroupSelects() const{return false;}
   virtual bool supportBinaryType () const { return m_BinarySupported; }
   virtual void setSupportBinaryType (bool Tran){ m_BinarySupported = Tran; }

private:
   void recreateSqlInsertWithoutBinding( const DBsqlInsert& SqlCommand, const COLstring& SqlString, COLostream& Output) const;

   DBresultSetPtr executeSqlInsertOrUpdate( const COLstring& SqlStr, const DBsqlInsert& SqlInsertCommand ) const;

   DBdataType sqliteTypeToDbType(int Type) const;

   COLostream& streamName( COLostream& Stream, const COLstring& Name, bool QuoteName ) const;

   void streamCreateTableNotNull( COLostream& Stream, 
                                          const DBsqlCreateTable& SqlCreateTableCommand, 
                                          COLuint32 ColumnIndex ) const;

   bool m_BinarySupported;
   mutable SQLsqlite m_Sqlite;
   DBSQLdatabaseSqlite( DBSQLdatabaseSqlite& Orig ); // not allowed
   DBSQLdatabaseSqlite& operator=( DBSQLdatabaseSqlite& Orig ); // not allowed
};

//returns newly initialized COLdateTime if unconvertible
COLdateTime DBSQLdateTimeFromStr(const COLstring& DateStrFromDb);
DBdatabase* DBSQLcreateSqlite();
// Resolves DataSource to absolute path, using Location as the base directory if DataSource was a relative path 
COLstring DBSQLresolve(const COLstring& DataSource, const COLstring& Location);
#endif // end of defensive include
